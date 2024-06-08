function isMobile() {
  return /Mobi|Android|iPhone|iPad|iPod/i.test(navigator.userAgent);
}

const joysticks = {};

function setupJoystick(joystickId) {
  const joystick = document.getElementById(joystickId);
  console.log("Joystick setup - " + joystickId);
  if (joystick) {
    const handle = joystick.querySelector(".joystick-handle");
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

      if (moveX >= 0) {
        moveX = Math.pow(moveX / maxMove, 0.5);
      } else {
        moveX = -Math.pow(-moveX / maxMove, 0.5);
      }

      if (moveY >= 0) {
        moveY = Math.pow(moveY / maxMove, 0.5);
      } else {
        moveY = -Math.pow(-moveY / maxMove, 0.5);
      }

      joysticks[joystickId] = {
        x: moveX,
        y: moveY * -1.0,
      };
    }

    function handleTouchEnd() {
      joysticks[joystickId] = { x: 0, y: 0 };
    }

    joystick.addEventListener("touchstart", handleTouchStart);
    joystick.addEventListener("touchmove", handleTouchMove);
    joystick.addEventListener("touchend", handleTouchEnd);
  }
}

document.addEventListener(
  "touchmove",
  function (event) {
    if (event.scale !== 1) {
      event.preventDefault();
    }
  },
  { passive: false }
);

export function getJoystickValues(joystickId) {
  return joysticks.hasOwnProperty(joystickId)
    ? joysticks[joystickId]
    : undefined;
}

export function enableJoysticks() {
  if (isMobile()) {
    var joystick = document.getElementById("joystickLeft");
    if (joystick) {
      joystick.style.display = "block";
    }
    joystick = document.getElementById("joystickRight");
    if (joystick) {
      joystick.style.display = "block";
    }
  }
}

export function disableJoysticks() {
  var joystick = document.getElementById("joystickLeft");
  if (joystick) {
    joystick.style.display = "none";
  }
  joystick = document.getElementById("joystickRight");
  if (joystick) {
    joystick.style.display = "none";
  }
}

if (isMobile()) {
  setupJoystick("joystickLeft");
  setupJoystick("joystickRight");
}
