import * as PIXI from "https://cdn.skypack.dev/pixi.js";
import * as PUI from "https://cdn.skypack.dev/@pixi/ui";

const uiContainer = new PIXI.Container();

let mainMenuElements = [];
let isMainMenuActive = false;

export function init(app) {
  app.stage.addChild(uiContainer);
}

function createMainMenu() {
  const button = new PUI.FancyButton({
    defaultView: "avatar_head_1",
    hoverView: "avatar_head_2",
    pressedView: "avatar_head_3",
    text: "Click me!",
    offset: {
      x: 100,
      y: 100,
    },
    animations: {
      hover: {
        props: {
          scale: {
            x: 1.1,
            y: 1.1,
          },
        },
        duration: 100,
      },
      pressed: {
        props: {
          scale: {
            x: 0.9,
            y: 0.9,
          },
        },
        duration: 100,
      },
    },
  });

  button.anchor.set(0.5, 0.5);
  button.x = 100;
  button.y = 100;

  button.onPress.connect(() => console.log("Button pressed!"));
  mainMenuElements.push(button);

  uiContainer.addChild(button);
}

export function showMainMenu() {
  if (mainMenuElements.length === 0) {
    createMainMenu();
  }
}

export function hideMainMenu() {}
