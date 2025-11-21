import { Vec } from "./Utils/Vector.js"
import { MapHandler } from "./Renderer/MapHandler.js";
import { ImgAsset } from "./Renderer/ImgAsset.js";
import { SoundManager } from "./SoundManager.js";
// import { sendPuzzleButtonPress, showPuzzleFeedback } from '../ButtonPuzzle/buttonPuzzle.js';


let canvas = document.getElementById("cnv");

canvas.width = window.innerWidth;
canvas.height = window.innerHeight;

let ctx = canvas.getContext("2d");
let ws;
let MAP = null;
let heldItem = null;
const invAssets = {};
let inventorySquare = new ImgAsset("inventorySquare", "js/Renderer/TileAssets/InventorySquare.png", Math.floor(canvas.width / 30), Math.floor(canvas.width / 30));
let wasMoving = false;

let itemDirectory = "js/Renderer/TileAssets/Blank.png";

ws = new WebSocket("ws://127.0.0.1:8080");
window.ws = ws;

const sound = new SoundManager();
window.sound = sound;
// sound.playAmbience();

ws.onmessage = (msg) => {
    let datStr = msg.data;
    let dat = JSON.parse(datStr);


    if (dat.tag == "alert") {
        alert(dat.msg);

        // Clear all held keys after alert is dismissed
        for (let k in hKey) {
            hKey[k] = false;
        }

        // This stops the player from trailing in the last keyPressed direction
        updDir(true);
    }


    if (dat.tag == "map") {
        MAP = new MapHandler(JSON.stringify(dat));
        MAP.setMappings(canvas, datStr);
        //draw(); // This doesn't seem to change anything so I'm commenting it out
        return;
    }

    if (dat.tag == "mov") {
        player = new Vec(dat.data.x, dat.data.y);
    }

    if (dat.tag === "puzzle_feedback") {
        showPuzzleFeedback(dat.feedback);
    }

    if (dat.tag == "inventory") {
        heldItem = dat.item;
        
        // Based on what player is holding, change the inventory box to that picture
        if (heldItem == "crowbar")
        {
            itemDirectory = "js/Renderer/TileAssets/Crowbar.png";
        }
        else if (heldItem == "joystick")
        {
            itemDirectory = "js/Renderer/TileAssets/Joystick.png";
        }

        if (heldItem && !invAssets[heldItem]) {
            let spriteSize = Math.floor(canvas.width / 30);
            invAssets[heldItem] = new ImgAsset(heldItem, itemDirectory, spriteSize, spriteSize);
        }
    }
    
    if(dat.tag == "update_tile"){
        console.log(dat);
        let tile = dat.tile;
        MAP.map[tile.z][tile.x][tile.y] = tile.type;
    }

    if (dat.tag === "button_feedback") {
        sound.playButton();
    }

    if (dat.tag === "break") {
        sound.playBreak();
    }

};

// Once websocket opens, load the start map
ws.addEventListener("open", () => {
    startMap();
    draw();
    alert("...Where am I? What is this place? \n" +
            "(Use the arrow keys or WASD to move around)\n" +
            "(Press E to interact with an item)");
})

// Function to load the map at the beginning of the game
function startMap(){
    send({
        tag: "start",
        data: {
            "width": canvas.width,
            "height": canvas.height,
        }
    })
}

const keyDirMap = {
    KeyW: new Vec([0, -1]),
    KeyS: new Vec([0, 1]),
    KeyA: new Vec([-1, 0]),
    KeyD: new Vec([1, 0]),
    ArrowUp: new Vec([0, -1]),
    ArrowDown: new Vec([0, 1]),
    ArrowLeft: new Vec([-1, 0]),
    ArrowRight: new Vec([1, 0]),
}

const hKey = {};
window.addEventListener("keydown", (e) => {
    hKey[e.code] = true;
    if(e.code == "KeyE"){
        interact();
    }
    else if(e.code == "KeyQ"){
        swapItem();
    }
    else {
        updDir(false);
    }
})
window.addEventListener("keyup", (e) => {
    hKey[e.code] = false;
    if (e.code != "KeyE" && e.code != "KeyQ")
    {
        updDir(true);
    }
})
function updDir(isUp){
    var dV = new Vec([0, 0]);
    for(let k in hKey){
        if(hKey[k] && keyDirMap[k]){
            dV.add(keyDirMap[k]);
        }
    }

    let dir = Math.atan2(dV.x, dV.y);
    let mov = Boolean(dV.magn());

    send({
        tag: "mov",
        data: {
            dir: dir,
            mov: Boolean(dV.magn()),
            isUp: isUp
        }
    
    })

    if (mov && !wasMoving) {
        sound.playMove();
    }
    wasMoving = mov
}

function interact(){
    send({
        tag: "interact",
        data: {
            "width": canvas.width,
            "height": canvas.height,
        }
    })
}

function showPuzzleFeedback(message) {
    const feedbackDiv = document.getElementById("puzzle-feedback");
    if (feedbackDiv) feedbackDiv.innerText = message;
}

function swapItem(){
    send({
        tag: "swapItem"
    })
}

function send(obj){
    ws.send(JSON.stringify(obj));
}

const TPS = 20;
let tInt = setInterval(() => {
    send({
        tag: "tick",
        data: {
            "width": canvas.width,
            "height": canvas.height,
        }
    })
}, 1e3 / TPS)
ws.onclose = () => {clearInterval(tInt)};

let player = new Vec(200, 200);
let image = new Image();
image.src = "../Assets/stickman.png"
image.onload = function() {
  draw();
};
//ctx.drawImage(image.src, player.x, player.y);

draw();
function draw(){
    let spriteSize = Math.floor(canvas.width / 30);

    ctx.clearRect(0, 0, canvas.width, canvas.height);
    
    inventorySquare.draw(ctx, 50, 50);

    if (MAP) {
        MAP.showMap(canvas, ctx, 0, 0);
        drawFrame(0, 0, player.x - canvas.width/50, player.y - canvas.height/15);
    }

    // ctx.beginPath();
    // ctx.arc(player.x, player.y,10,  0, Math.PI * 2);
    // ctx.strokeStyle = "white";
    // ctx.stroke();
    

    if (heldItem)
    {
        let img = invAssets[heldItem];
        if (!img)
        {
            // invAssets[heldItem] = new ImgAsset(heldItem, itemDirectory, spriteSize, spriteSize);
            img = invAssets[heldItem];
        }
        img.draw(ctx, 50, 50);
    }
    
    window.requestAnimationFrame(draw);
}

let character = new Image();
character.src = "./Assets/stickman_white.png"
character.onload = function() {
    step();
};

let spriteSize2 = Math.floor(canvas.width / 30);
let scale = 0.01 * spriteSize2;
const cWidth = 124;
const cHeight = 124;
const scaledWidth = scale * cWidth;
const scaledHeight = scale * cHeight;

function drawFrame(frameX, frameY, canvasX, canvasY) {
  ctx.drawImage(character,
                frameX * cWidth, frameY * cHeight, cWidth, cHeight,
                canvasX, canvasY, scaledWidth, scaledHeight);
}

const cycleLoop = [0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3];
let currentLoopIndex = 0;
let frameCount = 0;



/*
I just left this code here just in case we wanted animations.
It kind of works, but is very buggy - Luc
*/

// function step() {
//   frameCount++;
//   if (frameCount < 1) {
//     window.requestAnimationFrame(step);
//     return;
//   }
//   frameCount = 0;
//   //ctx.clearRect(0, 0, canvas.width, canvas.height);
//   drawFrame(cycleLoop[currentLoopIndex], 0, player.x - 20, player.y - 30);
//   currentLoopIndex++;
//   if (currentLoopIndex >= cycleLoop.length) {
//     currentLoopIndex = 0;
//   }
//   window.requestAnimationFrame(step);
// }
// class Sprite {
//   constructor(src, width, height, frames) {
//     this.image = new Image();
//     this.image.src = src;
//     this.width = width;
//     this.height = height;
//     this.frames = frames;
//   }
// }

// class AnimationHandler {
//   constructor(sprites) {
//     this.sprites = sprites;
//     this.loop = true;
//     this.sprites.image.onload = this.render(player.x, player.y);
//   }
//   async render(x, y) {
//     for (let i = 0; i < this.sprites.frames; i++) {
//       ctx.clearRect(0, 0, canvas.width, canvas.height);
//       ctx.drawImage(this.sprites.image, (this.sprites.width * i), 0, this.sprites.width, 
//       this.sprites.height, x, y, this.sprites.width * 0.3, this.sprites.height * 0.3);

//       await sleep(80)
//     }
//     if (this.loop) {this.render(player.x, player.y);}
//   }
// }

// function sleep(ms){
//     return new Promise(resolve => setTimeout(resolve, ms));
// }

// const anim1 = new Sprite("./Assets/stickman_white.png", 124, 124, 4);
// new AnimationHandler(anim1);
