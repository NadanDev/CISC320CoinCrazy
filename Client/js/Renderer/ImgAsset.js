export class ImgAsset{
    constructor(name, url, width, height){
        this.name = name;
        this.width = width;
        this.height = height;
        this.loaded = false;
        this.img = new Image(width, height);
        this.img.onload = () => {
            this.loaded = true;
        };
        this.img.onerror = () => {
            console.error("Failed to load image:", url);
        };
        this.img.src = url;
        
    }
    draw(ctx, x = 0, y = 0){
        if(!this.loaded){
            ctx.save();
            
            ctx.strokeStyle = "red";
            ctx.beginPath();
            ctx.rect(0, 0, this.width, this.height);
            ctx.moveTo(0, 0);
            ctx.lineTo(this.width, this.height)
            ctx.moveTo(0, this.height);
            ctx.lineTo(this.width, 0);
            ctx.stroke();
            
            ctx.restore();
            return;
        }

        ctx.save();
        ctx.drawImage(this.img, x, y, this.width, this.height);  
        ctx.restore();
    }
}