import * as PIXI from 'https://cdn.skypack.dev/pixi.js';

let assets = []
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

function add(id, alias, sizeX, sizeY, tint) 
{
    const asset = {
        id: id,
        alias: alias,
        x: 0,
        x: 0,
        smoothX: 0,
        smoothY: 0,
        size: {
            x: sizeX,
            y: sizeY
        },
        rot: 0,
        sprite: PIXI.Sprite.from(alias),
    }
        
    asset.sprite.anchor.set(0.5);
    asset.sprite.tint = tint;

    assetContainer.addChild(asset.sprite);
    assets.push(asset);

    return asset;
}

export function update(id, alias, x, y, sizeX, sizeY, rot, tint)
{
    let foundObject = assets.find(obj => obj.id === id);

    if (foundObject === undefined)
    {
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

export function remove(id)
{
    const foundObject = assets.find(obj => obj.id === id);

    if (foundObject !== undefined)
    {
        assetContainer.removeChild(foundObject.sprite);
        foundObject.sprite.destroy();
    
        assets = assets.filter(obj => obj.id !== id);

        bounds.max.x = -10000.0;
        bounds.max.y = -10000.0;
        bounds.min.x = 10000.0;
        bounds.min.y = 10000.0;
    }
}

export function getBounds()
{
    if (bounds.max.x === -10000.0)
    {    
        assets.forEach((asset) => {
            if (asset.alias.includes('static') === true)
            {
                bounds.max.x = Math.max(bounds.max.x, asset.x);
                bounds.max.y = Math.max(bounds.max.y, asset.y);
                bounds.min.x = Math.min(bounds.min.x, asset.x);
                bounds.min.y = Math.min(bounds.min.y, asset.y);
            }
        });
    }

    return bounds;
}

function lerp(start, end, t) {
    return start * (1 - t) + end * t;
}

export function interpolate(deltaTime)
{
    assets.forEach((asset) => {
        asset.smoothX = lerp(asset.smoothX, asset.x, deltaTime * 2.0);
        asset.smoothY = lerp(asset.smoothY, asset.y, deltaTime * 2.0);
    });
}

export function adjustAssetsView(scaleFactor, width, height)
{
    const padding = 0.95;

    assets.forEach((asset) => {
        asset.sprite.x = width * (1.0 - padding) / 2 + ((asset.smoothX - bounds.min.x) * scaleFactor * padding);
        asset.sprite.y = height * (1.0 - padding) / 2 + (height - ((asset.smoothY - bounds.min.y) * scaleFactor)) * padding;

        asset.sprite.width =  asset.size.x * scaleFactor; 
        asset.sprite.height =  asset.size.y * scaleFactor; 
    });
}