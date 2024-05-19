import * as CONNECTION from './connection.js'
import * as PIXI from 'https://cdn.skypack.dev/pixi.js';
import * as ASSETS from './assets.js';
import * as JOYSTICKS from './joystick.js'

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
        ASSETS.interpolate(time.deltaTime);
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
    await app.init({ backgroundAlpha: 0, resizeTo: window, antialias: true, autoDensity: true });

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
        { alias: 'block_default', src: './assets/block_default.png' },
        { alias: 'block_static', src: './assets/block_static.png' },
        { alias: 'block_tough', src: './assets/block_tough.png' },
        { alias: 'block_weak', src: './assets/block_weak.png' },
        { alias: 'block_floor_decor', src: './assets/block_floor_decor.png' },
        { alias: 'block_floor_decor2', src: './assets/block_floor_decor2.png' },
        { alias: 'block_roof_decor', src: './assets/block_roof_decor.png' },
        { alias: 'avatar_head', src: './assets/avatar_head.png' },
        { alias: 'avatar_body', src: './assets/avatar_body.png' },
        { alias: 'avatar_legs', src: './assets/avatar_legs.png' },
        { alias: 'weapon_shaft', src: './assets/weapon_shaft.png' },
        { alias: 'weapon_head', src: './assets/weapon_head.png' },
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
        ASSETS.update(asset.id, asset.alias, asset.x, asset.y, asset.sizeX, asset.sizeY, asset.rot, asset.tint);
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

    const joystickInputLeft = JOYSTICKS.getJoystickValues('joystickLeft');
    const joystickInputRight = JOYSTICKS.getJoystickValues('joystickRight');

    let joystickInput = false;
    if (joystickInputLeft !== undefined && joystickInputRight !== undefined )
    {
        if(joystickInputLeft.x !== 0.0 || joystickInputLeft.y !== 0.0 || joystickInputRight.x !== 0.0 || joystickInputRight.y !== 0.0 )
        {
            joystickInput = true;
            moveInput = joystickInputLeft.x;
            jumpInput = joystickInputLeft.y;
            
            sledgeInput = joystickInputRight.x;
        }
    }
    
    if (joystickInput == false)
    {
        if (keysPressed['ArrowRight'])
            {
                sledgeInput  = 1.0;
            };
            if (keysPressed['ArrowLeft'])
            {
                sledgeInput  = -1.0;
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
    }

    let input = {
        sledge: sledgeInput,
        move: moveInput,
        jump: jumpInput 
    }

    CONNECTION.sendInput(input);
}
