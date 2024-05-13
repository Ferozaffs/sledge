import * as PIXI from 'https://cdn.skypack.dev/pixi.js';
import * as BLOCKS from './blocks.js';

const app = new PIXI.Application();
let initialized = false;

// Asynchronous IIFE
(async () =>
{
    await init();
    await preload();

    initialized = true;
})();

async function init()
{
    // Intialize the application.
    await app.init({ background: '#888888', resizeTo: window });

    // Then adding the application's canvas to the DOM body.
    document.body.appendChild(app.canvas);

    BLOCKS.init(app);
}

async function preload()
{
    // Create an array of asset data to load.
    const assets = [
        { alias: 'block_basic', src: './assets/block_basic.png' },
    ];

    // Load the assets defined above.
    await PIXI.Assets.load(assets);
}

export async function addData(json)
{
    while (!initialized) {
        await new Promise(resolve => setTimeout(resolve, 100));
    }

    const blocks = json.blocks;

    // Loop over the array using forEach
    blocks.forEach((block) => {
        BLOCKS.addBlock(app, block.id, block.alias, block.x, 800.0-block.y);
    });
}

