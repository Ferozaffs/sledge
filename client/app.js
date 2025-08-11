import * as PIXI from "https://cdn.skypack.dev/pixi.js";
import * as ASSETS from "./assets.js";
import { sendInput } from "./connection.js";
import { getJoystickValues } from "./joystick.js";
import * as BYTEREADER from "./bytereader.js";

const app = new PIXI.Application();
let initialized = false;
let keysPressed = {};
let tickCounter = 0.0;

(async () => {
  await init();
  await preload();

  initialized = true;

  app.ticker.add((time) => {
    updateView();

    tickCounter += time.deltaTime;
    if (tickCounter > 1.0 / 60.0) {
      sendData();
      tickCounter = 0.0;
    }
  });
})();

async function init() {
  await app.init({
    backgroundAlpha: 0,
    resizeTo: window,
    antialias: true,
    autoDensity: true,
  });

  document.body.appendChild(app.canvas);
  document.addEventListener("keydown", (event) => {
    keysPressed[event.key] = true;
  });

  document.addEventListener("keyup", (event) => {
    delete keysPressed[event.key];
  });

  window.addEventListener("blur", () => {
    keysPressed = {};
  });

  ASSETS.init(app);
}

async function preload() {
  const assets = [
    { alias: "block_default", src: "./assets/block_default.png" },
    { alias: "block_static", src: "./assets/block_static.png" },
    { alias: "block_tough", src: "./assets/block_tough.png" },
    { alias: "block_weak", src: "./assets/block_weak.png" },
    { alias: "block_floor_decor_1", src: "./assets/block_floor_decor_1.png" },
    { alias: "block_floor_decor_2", src: "./assets/block_floor_decor_2.png" },
    { alias: "block_floor_decor_3", src: "./assets/block_floor_decor_3.png" },
    { alias: "block_roof_decor_1", src: "./assets/block_roof_decor_1.png" },
    { alias: "block_roof_decor_2", src: "./assets/block_roof_decor_2.png" },
    { alias: "avatar_head_1", src: "./assets/avatar_head_1.png" },
    { alias: "avatar_head_2", src: "./assets/avatar_head_2.png" },
    { alias: "avatar_head_3", src: "./assets/avatar_head_3.png" },
    { alias: "avatar_crown", src: "./assets/avatar_crown.png" },
    { alias: "avatar_body", src: "./assets/avatar_body.png" },
    { alias: "avatar_legs", src: "./assets/avatar_legs.png" },
    { alias: "weapon_shaft", src: "./assets/weapon_shaft.png" },
    { alias: "weapon_head", src: "./assets/weapon_head.png" },
    { alias: "koth_zone", src: "./assets/koth_zone.png" },
    { alias: "koth_zone_red", src: "./assets/koth_zone_red.png" },
    { alias: "koth_zone_blue", src: "./assets/koth_zone_blue.png" },
  ];

  await PIXI.Assets.load(assets);
  await PIXI.Assets.load("./assets/ChakraPetch.ttf");
}

export async function createAsset(bytes, offset, size) {
  while (!initialized) {
    await new Promise((resolve) => setTimeout(resolve, 100));
  }

  while (size > 0) {
    let id = BYTEREADER.readInt32(bytes, offset);
    offset += 4;
    size -= 4;
    let x = BYTEREADER.readFloat32(bytes, offset);
    offset += 4;
    size -= 4;
    let y = BYTEREADER.readFloat32(bytes, offset);
    offset += 4;
    size -= 4;
    let rot = BYTEREADER.readFloat32(bytes, offset);
    offset += 4;
    size -= 4;
    let sizeX = BYTEREADER.readFloat32(bytes, offset);
    offset += 4;
    size -= 4;
    let sizeY = BYTEREADER.readFloat32(bytes, offset);
    offset += 4;
    size -= 4;
    let tint = BYTEREADER.readUInt32(bytes, offset);
    offset += 4;
    size -= 4;
    let aliasLength = BYTEREADER.readUInt16(bytes, offset);
    offset += 2;
    size -= 2;
    let alias = BYTEREADER.readString(bytes, offset, aliasLength);
    offset += aliasLength;
    size -= aliasLength;

    ASSETS.create(id, alias, x, y, sizeX, sizeY, rot, tint);
  }
}

export async function updateAsset(bytes, offset, size) {
  while (!initialized) {
    await new Promise((resolve) => setTimeout(resolve, 100));
  }
  const count = size / 16;
  for (let i = 0; i < count; i++) {
    let id = BYTEREADER.readUInt32(bytes, offset);
    offset += 4;
    let x = BYTEREADER.readFloat32(bytes, offset);
    offset += 4;
    let y = BYTEREADER.readFloat32(bytes, offset);
    offset += 4;
    let rot = BYTEREADER.readFloat32(bytes, offset);
    offset += 4;

    ASSETS.update(id, x, y, rot);
  }
}

export async function removeAsset(bytes, offset, size) {
  while (!initialized) {
    await new Promise((resolve) => setTimeout(resolve, 100));
  }
  const count = size / 4;
  for (let i = 0; i < count; i++) {
    let id = BYTEREADER.readUInt32(bytes, offset);
    offset += 4;

    ASSETS.remove(id);
  }
}

export async function updateScore(bytes, offset, size) {
  while (!initialized) {
    await new Promise((resolve) => setTimeout(resolve, 100));
  }

  const count = size / 8;
  for (let i = 0; i < count; i++) {
    let id = BYTEREADER.readUInt32(bytes, offset);
    offset += 4;
    let score = BYTEREADER.readUInt16(bytes, offset);
    offset += 2;

    ASSETS.setScore(id, score);
  }
}

function updateView() {
  const width = document.body.clientWidth;
  const height = document.body.clientHeight;
  const assetBounds = ASSETS.getBounds();

  const scaleFactorW = width / (assetBounds.max.x - assetBounds.min.x);
  const scaleFactorH = height / (assetBounds.max.y - assetBounds.min.y);
  ASSETS.adjustAssetsView(Math.min(scaleFactorW, scaleFactorH), width, height);
}

function sendData() {
  let sledgeInput = 0.0;
  let jumpInput = 0.0;
  let moveInput = 0.0;

  const joystickInputLeft = getJoystickValues("joystickLeft");
  const joystickInputRight = getJoystickValues("joystickRight");

  let joystickInput = false;
  if (joystickInputLeft !== undefined && joystickInputRight !== undefined) {
    if (
      joystickInputLeft.x !== 0.0 ||
      joystickInputLeft.y !== 0.0 ||
      joystickInputRight.x !== 0.0 ||
      joystickInputRight.y !== 0.0
    ) {
      joystickInput = true;
      moveInput = joystickInputLeft.x;
      jumpInput = joystickInputLeft.y;

      sledgeInput = joystickInputRight.x;
    }
  }

  if (joystickInput == false) {
    if (keysPressed["ArrowRight"]) {
      sledgeInput = 1.0;
    }
    if (keysPressed["ArrowLeft"]) {
      sledgeInput = -1.0;
    }

    if (keysPressed["w"]) {
      jumpInput = 1.0;
    }

    if (keysPressed["s"]) {
      jumpInput = -1.0;
    }

    if (keysPressed["a"]) {
      moveInput = -1.0;
    }
    if (keysPressed["d"]) {
      moveInput = 1.0;
    }
  }

  let input = {
    sledge: sledgeInput,
    move: moveInput,
    jump: jumpInput,
  };

  sendInput(input);
}
