#include "WaterBody.hpp"

#include <random>
#include <functional>
#include <SFML/OpenGL.hpp>

#include <assert.hpp>
#include <chronoUtils.hpp>

constexpr float MaxSineWaveHeight = 4;
constexpr float MaxSineWaveK = 0.08f;
constexpr float MaxSineWaveOmega = 0.08f;

constexpr auto WaveDrawingShader = R"wavedraw(

uniform sampler2D dynamicTex;
uniform vec4 color, coastColor;
uniform vec4 staticWaveProperties[8];
uniform float t;

void main()
{
    float x = gl_TexCoord[0].x;
    float staticDist = 0.0;
    for (int i = 0; i < 8; i++)
    {
        staticDist += staticWaveProperties[i].x + staticWaveProperties[i].y *
                      sin(staticWaveProperties[i].z * x + staticWaveProperties[i].w * t);
    }

    float weight = step(staticDist, gl_TexCoord[0].y);
    float mixed = step(staticDist + 4.0, gl_TexCoord[0].y);

    gl_FragColor = mix(coastColor, color, mixed) * weight;
}
)wavedraw";

sf::Shader& WaterBody::getDrawingShader()
{
    static sf::Shader shader;
    static bool shaderLoaded = false;

    if (!shaderLoaded)
    {
        ASSERT(shader.loadFromMemory(WaveDrawingShader, sf::Shader::Fragment));
        shaderLoaded = true;
    }

    return shader;
}

WaterBody::WaterBody(sf::Vector2f drawingSize) : drawingSize(drawingSize), curT(0)
{
    recreateQuad();
    resetWaves();
}

void WaterBody::recreateQuad()
{
    quad[0].position = quad[0].texCoords = sf::Vector2f(0, -256);
    quad[1].position = quad[1].texCoords = sf::Vector2f(0, drawingSize.y);
    quad[2].position = quad[2].texCoords = sf::Vector2f(drawingSize.x, drawingSize.y);
    quad[3].position = quad[3].texCoords = sf::Vector2f(drawingSize.x, -256);
    
    if (topHidden)
        quad[0].position.y = quad[3].position.y = 0;

    for (auto& vtx : quad)
        vtx.color = sf::Color::White;
}

void WaterBody::resetWaves()
{
    if (!topHidden)
    {
        std::random_device dev;

        std::mt19937_64 gen(dev());
        std::uniform_real_distribution<float> dist;
        auto random = std::bind(dist, gen);

        for (auto& wave : generatedWaves)
        {
            wave.ofs = MaxSineWaveHeight * (2.0f * random() - 1.0f);
            wave.amp = MaxSineWaveHeight * random();
            wave.k = MaxSineWaveK * random();
            wave.omega = MaxSineWaveOmega * (2.0f * random() - 1.0f);
        }
    }
    else
    {
        for (auto& wave : generatedWaves)
        {
            wave.ofs = -256;
            wave.amp = wave.k = wave.omega = 0;
        }
    }
}


void WaterBody::update(std::chrono::steady_clock::time_point curTime)
{
    if (startingTime == decltype(startingTime)())
    {
        curT = 0;
        startingTime = curTime;
    }

    curT = (curTime - startingTime)/UpdateFrequency;
}

void WaterBody::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    auto& shader = getDrawingShader();
    shader.setUniform("color", sf::Glsl::Vec4(color));
    shader.setUniform("coastColor", sf::Glsl::Vec4(coastColor));
    shader.setUniformArray("staticWaveProperties", reinterpret_cast<const sf::Glsl::Vec4*>(generatedWaves), 8);
    shader.setUniform("t", (float)curT);

    states.shader = &shader;

    target.draw(quad, 4, sf::TriangleFan, states);
}
