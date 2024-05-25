const joysticks = {};

function setupJoystick(joystickId) {
    const joystick = document.getElementById(joystickId);
    const handle = joystick.querySelector('.joystick-handle');
    let startX, startY, moveX, moveY;

    joysticks[joystickId] = { x: 0, y: 0 };

    function handleTouchStart(event) {
        const touch = event.targetTouches[0];
        startX = touch.clientX;
        startY = touch.clientY;
    }

    function handleTouchMove(event) {
        const touch = event.targetTouches[0];
        moveX = touch.clientX - startX;
        moveY = touch.clientY - startY;
        const maxMove = 40;
        const distance = Math.sqrt(moveX * moveX + moveY * moveY);
        if (distance > maxMove) {
            const angle = Math.atan2(moveY, moveX);
            moveX = Math.cos(angle) * maxMove;
            moveY = Math.sin(angle) * maxMove;
        }
        
        joysticks[joystickId] = {
            x: Math.pow(moveX / maxMove, 0.5),
            y: Math.pow(moveY / maxMove, 0.5) * -1.0
        };
    }

    function handleTouchEnd() {
        joysticks[joystickId] = { x: 0, y: 0 };
    }

    joystick.addEventListener('touchstart', handleTouchStart);
    joystick.addEventListener('touchmove', handleTouchMove);
    joystick.addEventListener('touchend', handleTouchEnd);
}

document.addEventListener('touchmove', function(event) {
    if (event.scale !== 1) { 
        event.preventDefault();
    }
}, { passive: false });

setupJoystick('joystickLeft');
setupJoystick('joystickRight');

export function getJoystickValues(joystickId) {
    return joysticks.hasOwnProperty(joystickId) ? joysticks[joystickId] : undefined;
}