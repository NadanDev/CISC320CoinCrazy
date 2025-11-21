import { AssetHandler } from "./AssetHandler.js";

const AH = new AssetHandler();

class MapHandler{
    constructor(json){
        let obj = JSON.parse(json);
        this.map = obj.map;
        
        this.height = this.map.length;
        this.width = this.map[0].length;
        this.len = this.map[0][0].length;
        
        this.mappings = {};
    }
    
    setMappings(canvas, json){
        let obj = JSON.parse(json);
        let spriteSize = Math.floor(canvas.width / 30);

        AH.updateImg("+", "js/Renderer/TileAssets/Floor.png", spriteSize, spriteSize);
        AH.updateImg("a", "js/Renderer/TileAssets/Blank.png", spriteSize, spriteSize);
        AH.updateImg("=", "js/Renderer/TileAssets/TopWall.png", spriteSize, spriteSize);
        AH.updateImg("[", "js/Renderer/TileAssets/LeftWall.png", spriteSize, spriteSize);
        AH.updateImg("]", "js/Renderer/TileAssets/RightWall.png", spriteSize, spriteSize);
        AH.updateImg("^", "js/Renderer/TileAssets/BottomWall.png", spriteSize, spriteSize);
        AH.updateImg("A", "js/Renderer/TileAssets/ArcadeMachine.png", spriteSize, spriteSize);
        AH.updateImg("B", "js/Renderer/TileAssets/Box2.png", spriteSize, spriteSize);
        AH.updateImg("C", "js/Renderer/TileAssets/Crowbar.png", spriteSize, spriteSize);   
        AH.updateImg("P", "js/Renderer/TileAssets/PurpleButton.png", spriteSize, spriteSize);
        AH.updateImg("Q", "js/Renderer/TileAssets/RedButton.png", spriteSize, spriteSize);
        AH.updateImg("R", "js/Renderer/TileAssets/GreenButton.png", spriteSize, spriteSize);
        AH.updateImg("S", "js/Renderer/TileAssets/YellowButton.png", spriteSize, spriteSize);
        AH.updateImg("T", "js/Renderer/TileAssets/BlueButton.png", spriteSize, spriteSize);
        AH.updateImg("X", "js/Renderer/TileAssets/PaintedBox.png", spriteSize, spriteSize);
        AH.updateImg("J", "js/Renderer/TileAssets/Joystick.png", spriteSize, spriteSize);
        AH.updateImg("c", "js/Renderer/TileAssets/Coins.png", spriteSize, spriteSize);    


        for(let arr of obj.map){
            for(let pair0 of arr){
                for(let pair1 of pair0){
                    this.mappings[pair1] = pair1;
                }
            }
        }
    }
    
    showMap(canvas, ctx, x1, y1){
        let spriteSize = Math.floor(canvas.width / 30);
        let xPos = Math.round((canvas.width / 2) - (this.width * spriteSize / 2));
        let yPos = Math.round((canvas.height / 2) - (this.len * spriteSize / 2))
        for(let z = 0; z < this.height; z++){
            for(let x = 0; x < this.width; x++){
                for(let y = 0; y < this.len; y++){
                    if (z == 1 && this.mappings[this.map[z][x][y]] == "A"){
                        AH.draw(this.mappings[this.map[z][x][y]], ctx, xPos + x * spriteSize, yPos + y * spriteSize - spriteSize/2);
                    }
                    else{
                        AH.draw(this.mappings[this.map[z][x][y]], ctx, xPos + x * spriteSize, yPos + y * spriteSize);
                    }
                }
            }
        }
    }
}

export {
    MapHandler
}