#include "PlayerController.hpp"

PlayerController::PlayerController(InputManager &inputManager, const InputSettings &settings)
    : dash(active), jump(active), bomb(active), movement(active)
{
    dash.registerSource(inputManager, settings.keyboardSettings.dashInput);
    jump.registerSource(inputManager, settings.keyboardSettings.jumpInput);
    bomb.registerSource(inputManager, settings.keyboardSettings.bombInput);
    movement.registerButtonForX(inputManager, settings.keyboardSettings.moveLeft,  AxisDirection::Negative);
    movement.registerButtonForX(inputManager, settings.keyboardSettings.moveRight, AxisDirection::Positive);
    movement.registerButtonForY(inputManager, settings.keyboardSettings.moveUp,    AxisDirection::Negative);
    movement.registerButtonForY(inputManager, settings.keyboardSettings.moveDown,  AxisDirection::Positive);

    dash.registerSource(inputManager, settings.joystickSettings.dashInput);
    jump.registerSource(inputManager, settings.joystickSettings.jumpInput);
    bomb.registerSource(inputManager, settings.joystickSettings.bombInput);
    movement.registerAxisForX(inputManager, settings.joystickSettings.movementAxisX);
    movement.registerAxisForY(inputManager, settings.joystickSettings.movementAxisY);
}

void PlayerController::update()
{
    dash.update();
    jump.update();
    bomb.update();
}
