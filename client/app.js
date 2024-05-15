import * as CONNECTION from './connection.js'
import * as PIXI from 'https://cdn.skypack.dev/pixi.js';
import * as ASSETS from './assets.js';

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
        updateView();

        tickCounter += time.deltaTime;
        if (tickCounter > 1.0 / 60.0)
        {
            sendData();
            tickCounter = 0.0;
        }
    });
})();

async function init()
{
    await app.init({ background: '#888888', resizeTo: window });

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

      ASSETS.init(app);
}

async function preload()
{
    const assets = [
        { alias: 'block_basic', src: './assets/block_basic.png' },
    ];

    await PIXI.Assets.load(assets);
}

export async function updateData(json)
{
    while (!initialized) {
        await new Promise(resolve => setTimeout(resolve, 100));
    }

    const assets = json.assets;

    assets.forEach((asset) => {
        ASSETS.update(asset.id, asset.alias, asset.x, asset.y, asset.sizeX, asset.sizeY, asset.rot);
    });
}

export async function removeData(json)
{
    while (!initialized) {
        await new Promise(resolve => setTimeout(resolve, 100));
    }

    const assets = json.assets;

    assets.forEach((asset) => {
        ASSETS.remove(asset.id);
    });
}

function updateView()
{
    const width = document.body.clientWidth;
    const height = document.body.clientHeight
    const assetBounds = ASSETS.getBounds(); 
    
    const scaleFactorW = width / (assetBounds.max.x - assetBounds.min.x);
    const scaleFactorH = height / (assetBounds.max.y - assetBounds.min.y);
    ASSETS.adjustAssetsView(Math.min(scaleFactorW, scaleFactorH), width, height);
}

function sendData() 
{
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
