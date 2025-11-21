import { ImgAsset } from "./ImgAsset.js";

export class AssetHandler{
    constructor(){
        this.assets = {};
    }
    addImg(name, url, width, height){
        if(this.assets[name])return;
        this.updateImg(name, url, width, height);
    }
    updateImg(name, url, width, height){
        this.assets[name] = new ImgAsset(name, url, width, height);
    }
    draw(name, ctx, x, y){
    const asset = this.assets[name];
    if(!asset){
        console.warn("Missing asset for", name);
        return;
    }
    asset.draw(ctx, x, y);
}
}