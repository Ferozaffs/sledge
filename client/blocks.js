import * as PIXI from 'https://cdn.skypack.dev/pixi.js';

const blocks = []
const blockContainer = new PIXI.Container();

export function init(app)
{
    app.stage.addChild(blockContainer);
}

export function addBlock(app, id, blockAlias, x, y)
{
    const block = PIXI.Sprite.from(blockAlias);

    block.anchor.set(0.5);

    block.id = id;
    block.x = x;
    block.y = y;

    block.scale.set(1.0);

    blockContainer.addChild(block);
    blocks.push(block);
}