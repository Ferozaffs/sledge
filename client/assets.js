import * as PIXI from 'https://cdn.skypack.dev/pixi.js';

const assets = []
const assetContainer = new PIXI.Container();
const bounds = {
    max: {
        x: 0.0,
        y: 0.0
    },
    min: {
        x: 0.0,
        y: 0.0
    },
}

export function init(app)
{
    app.stage.addChild(assetContainer);
}

function add(id, alias, sizeX, sizeY) 
{
    const asset = {
        id: id,
        x: 0,
        x: 0,
        size: {
            x: sizeX,
            y: sizeY
        },
        rot: 0,
        sprite: PIXI.Sprite.from(alias),
    }
        
    asset.sprite.anchor.set(0.5);

    assetContainer.addChild(asset.sprite);
    assets.push(asset);

    return asset;
}

export function update(id, alias, x, y, sizeX, sizeY, rot)
{
    let foundObject = assets.find(obj => obj.id === id);

    if (foundObject === undefined)
    {
        foundObject = add(id, alias, sizeX, sizeY);
    }

    foundObject.x = x;
    foundObject.y = y;
    foundObject.sprite.rotation = -rot;

    bounds.max.x = -10000.0;
    bounds.max.y = -10000.0;
    bounds.min.x = 10000.0;
    bounds.min.y = 10000.0;
}

export function remove(id)
{
    const foundObject = assets.find(obj => obj.id === id);

    if (foundObject !== undefined)
    {
        assetContainer.removeChild(foundObject.sprite);
        foundObject.destroy();
    
        assets = assets.filter(obj => obj.id !== id);
    }

    bounds.max.x = -10000.0;
    bounds.max.y = -10000.0;
    bounds.min.x = 10000.0;
    bounds.min.y = 10000.0;
}

export function getBounds()
{
    if (bounds.max.x === -10000.0)
    {    
        assets.forEach((asset) => {
            bounds.max.x = Math.max(bounds.max.x, asset.x);
            bounds.max.y = Math.max(bounds.max.y, asset.y);
            bounds.min.x = Math.min(bounds.min.x, asset.x);
            bounds.min.y = Math.min(bounds.min.y, asset.y);
        });
    }

    return bounds;
}

export function adjustAssetsView(scaleFactor, width, height)
{
    const padding = 0.95;

    assets.forEach((asset) => {
        asset.sprite.x = width * (1.0 - padding) / 2 + ((asset.x - bounds.min.x) * scaleFactor * padding);
        asset.sprite.y = (height - ((asset.y - bounds.min.y) * scaleFactor)) * padding;

        asset.sprite.width =  asset.size.x * scaleFactor; 
        asset.sprite.height =  asset.size.y * scaleFactor; 
    });
}