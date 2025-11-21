export class SoundManager {
  constructor() {
    this.moveSound = null;
    this.breakSound = null;
    this.buttonSound = null;
    this.ambienceSound = null;

    soundManager.setup({
      url: '/ExternalLibs/SoundManager/swf/',
      onready: () => {
        this.moveSound = soundManager.createSound({
          id: 'move',
          url: '/js/SoundAssets/move.mp3',
          autoLoad: true,
        });
        this.breakSound = soundManager.createSound({
          id: 'break',
          url: '/js/SoundAssets/SwingAndBreak.mp3',
          autoLoad: true,
        });
        this.buttonSound = soundManager.createSound({
          id: 'press',
          url: '/js/SoundAssets/ButtonPress.mp3',
          autoLoad: true,
        });
        this.ambienceSound = soundManager.createSound({
          id: 'ambience',
          url: '/js/SoundAssets/Ambience.mp3',
          autoLoad: true,
          loops: true, 
        });
        soundManager.play('ambience');
      },
    });
  }

  playMove() {
    if (this.moveSound) {
      if (!this.moveSound.playState) {
          this.moveSound.play(); 
      }
    }
  }
  playBreak() {
    if (this.breakSound) {
      if (!this.breakSound.playState) {
          this.breakSound.play(); 
      }
    }
  }

  playButton() {
    if (this.buttonSound) {
      if (!this.buttonSound.playState) {
          this.buttonSound.play(); 
      }
    }
  }
}