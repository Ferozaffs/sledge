import * as PIXI from "https://cdn.skypack.dev/pixi.js";

let assets = [];
const assetContainer = new PIXI.Container();
const bounds = {
  max: {
    x: -10000.0,
    y: -10000.0,
  },
  min: {
    x: 10000.0,
    y: 10000.0,
  },
};
const staticBounds = {
  max: {
    x: -10000.0,
    y: -10000.0,
  },
  min: {
    x: 10000.0,
    y: 10000.0,
  },
};

export function init(app) {
  app.stage.addChild(assetContainer);
}

function add(id, alias, sizeX, sizeY, tint) {
  const asset = {
    id: id,
    alias: alias,
    x: 0,
    x: 0,
    smoothX: 0,
    smoothY: 0,
    size: {
      x: sizeX,
      y: sizeY,
    },
    rot: 0,
    sprite: PIXI.Sprite.from(alias),
    text: undefined,
  };

  asset.sprite.anchor.set(0.5);
  asset.sprite.tint = tint;

  assetContainer.addChild(asset.sprite);
  assets.push(asset);

  return asset;
}

export function create(id, alias, x, y, sizeX, sizeY, rot, tint) {
  let foundObject = assets.find((obj) => obj.id === id);

  if (foundObject === undefined) {
    if (alias === undefined) {
      return;
    }

    foundObject = add(id, alias, sizeX, sizeY, tint);

    bounds.max.x = -10000.0;
    bounds.max.y = -10000.0;
    bounds.min.x = 10000.0;
    bounds.min.y = 10000.0;
  }

  foundObject.smoothX = foundObject.x = x;
  foundObject.smoothY = foundObject.y = y;
  foundObject.sprite.rotation = -rot;
}

export function update(id, x, y, rot) {
  let foundObject = assets.find((obj) => obj.id === id);

  foundObject.smoothX = foundObject.x = x;
  foundObject.smoothY = foundObject.y = y;
  foundObject.sprite.rotation = -rot;
}

export function remove(id) {
  const foundObject = assets.find((obj) => obj.id === id);

  if (foundObject !== undefined) {
    assetContainer.removeChild(foundObject.sprite);
    foundObject.sprite.destroy();
    if (foundObject.text !== undefined) {
      assetContainer.removeChild(foundObject.text);
      foundObject.text.destroy();
    }

    assets = assets.filter((obj) => obj.id !== id);

    bounds.max.x = -10000.0;
    bounds.max.y = -10000.0;
    bounds.min.x = 10000.0;
    bounds.min.y = 10000.0;
  }
}

export function getBounds() {
  if (bounds.max.x === -10000.0) {
    assets.forEach((asset) => {
      if (
        asset.alias.includes("static") === true ||
        asset.alias.includes("weak") === true
      ) {
        staticBounds.max.x = Math.max(staticBounds.max.x, asset.x + 15);
        staticBounds.max.y = Math.max(staticBounds.max.y, asset.y + 15);
        staticBounds.min.x = Math.min(staticBounds.min.x, asset.x - 15);
        staticBounds.min.y = Math.min(staticBounds.min.y, asset.y - 5);
      }
    });

    bounds.max.x = staticBounds.max.x;
    bounds.max.y = staticBounds.max.y;
    bounds.min.x = staticBounds.min.x;
    bounds.min.y = staticBounds.min.y;
  }

  //let avatarBounds = {
  //    max: {
  //        x: -10000.0,
  //        y: -10000.0
  //    },
  //    min: {
  //        x: 10000.0,
  //        y: 10000.0
  //    },
  //}
  //
  //assets.forEach((asset) => {
  //    if(asset.alias.includes('avatar') === true)
  //    {
  //        avatarBounds.max.x = Math.max(avatarBounds.max.x, asset.x + 10);
  //        avatarBounds.max.y = Math.max(avatarBounds.max.y, asset.y + 10);
  //        avatarBounds.min.x = Math.min(avatarBounds.min.x, asset.x - 10);
  //        avatarBounds.min.y = Math.min(avatarBounds.min.y, asset.y - 10);
  //    }
  //});
  //
  //staticBounds.max.x = Math.min(staticBounds.max.x, staticBounds.min.x + 250);
  //staticBounds.max.y = Math.min(staticBounds.max.y, staticBounds.min.y + 250);
  //
  //bounds.max.x = Math.max(avatarBounds.max.x, staticBounds.max.x);
  //bounds.max.y = Math.max(avatarBounds.max.y, staticBounds.max.y);
  //bounds.min.x = Math.min(avatarBounds.min.x, staticBounds.min.x);
  //bounds.min.y = staticBounds.min.y;

  return bounds;
}

export function adjustAssetsView(scaleFactor, width, height) {
  const padding = 0.95;

  assets.forEach((asset) => {
    asset.sprite.x =
      (width * (1.0 - padding)) / 2 +
      (asset.smoothX - bounds.min.x) * scaleFactor * padding;
    asset.sprite.y =
      (height * (1.0 - padding)) / 2 +
      (height - (asset.smoothY - bounds.min.y) * scaleFactor) * padding;

    asset.sprite.width = asset.size.x * scaleFactor;
    asset.sprite.height = asset.size.y * scaleFactor;

    if (asset.text !== undefined) {
      asset.text.x = asset.sprite.x - 1 * scaleFactor;
      asset.text.y = asset.sprite.y - 10 * scaleFactor;
      asset.text.style.fontSize = 3 * scaleFactor;
    }
  });
}

export function setScore(id, score) {
  let foundObject = assets.find((obj) => obj.id === id);

  if (foundObject !== undefined) {
    if (foundObject.text === undefined) {
      foundObject.text = new PIXI.Text({
        text: "",
        style: {
          fontFamily: "ChakraPetch",
          fontSize: 25,
          align: "center",
          dropShadow: true,
          dropShadowAlpha: 0.7,
          dropShadowDistance: 2,
          fill: "#ffd500",
        },
      });

      assetContainer.addChild(foundObject.text);
    }

    foundObject.text.text = score;
  }
}
