import * as CONNECTION from './connection.js'
import * as PIXI from 'https://cdn.skypack.dev/pixi.js';
import * as BLOCKS from './blocks.js';

const app = new PIXI.Application();
let initialized = false;
let keysPressed = {};
let tickCounter = 0.0;

// Asynchronous IIFE
(async () =>
{
    await init();
    await preload();

    initialized = true;

    app.ticker.add((time) =>
    {
        tickCounter += time.deltaTime;
        if (tickCounter > 1.0 / 60.0)
        {
            SendData();
            tickCounter = 0.0;
        }
    });
})();

async function init()
{
    // Intialize the application.
    await app.init({ background: '#888888', resizeTo: window });

    // Then adding the application's canvas to the DOM body.
    document.body.appendChild(app.canvas);
    document.addEventListener('keydown', (event) => {
        keysPressed[event.key] = true;
      });
      
      document.addEventListener('keyup', (event) => {
        delete keysPressed[event.key];
      });
      
      window.addEventListener('blur', () => {
        keysPressed = {};
      });

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

    blocks.forEach((block) => {
        BLOCKS.addBlock(app, block.id, block.alias, block.x, 800.0-block.y);
    });

    const avatars = json.avatars;
    avatars.forEach((avatar) => {
        AVATARS.addPlayer(app, avatar.id, avatar.alias, avatar.x, 800.0-avatar.y);
    });
}

function SendData() {

    let sledgeInput = 0.0;
    let jumpInput = 0.0;
    let moveInput = 0.0;

    if (keysPressed['ArrowRight'])
    {
        sledgeInput  = -1.0;
    };
    if (keysPressed['ArrowLeft'])
    {
        sledgeInput  = 1.0;
    };

    if (keysPressed['w'])
    {
        jumpInput  = 1.0;
    };

    if (keysPressed['a'])
    {
        moveInput  = -1.0;
    };
    if (keysPressed['d'])
    {
        moveInput  = 1.0;
    };

    let input = {
        sledge: sledgeInput,
        move: moveInput,
        jump: jumpInput 
    }

    CONNECTION.sendInput(input);
}
