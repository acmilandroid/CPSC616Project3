#include <iostream>
#include <algorithm>
#include <sstream>
#include <string>
#include <random>
#include <iomanip>
#include "sprite.h"
#include "multisprite.h"
#include "twowaymultisprite.h"
#include "gameData.h"
#include "engine.h"
#include "frameGenerator.h"

Engine::~Engine() { 
  //for (auto& it : s) delete it;
  std::cout << "Terminating program" << std::endl;
}

Engine::Engine() :
  rc( RenderContext::getInstance() ),
  io( IoMod::getInstance() ),
  clock( Clock::getInstance() ),
  renderer( rc.getRenderer() ),
  sky("sky", Gamedata::getInstance().getXmlInt("sky/factor") ),
  flying("flying", Gamedata::getInstance().getXmlInt("flying/factor") ),
  cloud("cloud", Gamedata::getInstance().getXmlInt("cloud/factor") ),
  viewport( Viewport::getInstance() ),
  s(8),
  currentSprite(0),
  makeVideo( false )
{
  s[0] = new TwoWayMultiSprite("Pigeon");
  s[0]->setScale(.4);
  s[1] = new TwoWayMultiSprite("PigeonSlow");
  s[1]->setScale(.3);
  s[2] = new TwoWayMultiSprite("PigeonSlow2");
  s[2]->setScale(.4);
  s[3] = new TwoWayMultiSprite("PlaneBig");
  s[3]->setScale(.3);
  s[4] = new TwoWayMultiSprite("PlaneMedium");
  s[4]->setScale(.4);
  s[5] = new TwoWayMultiSprite("PlaneSmall");
  s[5]->setScale(.2);
  s[6] = new Sprite("Cloud");
  s[6]->setScale(.5);
  s[7] = new Sprite("Cloud2");
  s[7]->setScale(.5);
  Viewport::getInstance().setObjectToTrack(s[0]);
  std::cout << "Loading complete" << std::endl;
}

void Engine::draw() const {
  sky.draw();
  flying.draw();
  cloud.draw();

  for (auto& it : s) it->draw();

  viewport.draw();
  SDL_RenderPresent(renderer);
}

void Engine::update(Uint32 ticks) {
  for (auto& it : s) it->update(ticks);
  sky.update();
  flying.update();
  cloud.update();
  viewport.update(); // always update viewport last
}

void Engine::switchSprite(){
  ++currentSprite;
  currentSprite = currentSprite % s.size();
  Viewport::getInstance().setObjectToTrack(s[currentSprite]);
  //if ( currentSprite ) {
  //  Viewport::getInstance().setObjectToTrack(spinningStar);
  //}
  //else {
  // Viewport::getInstance().setObjectToTrack(star);
  //}
}

void Engine::play() {
  SDL_Event event;
  const Uint8* keystate;
  bool done = false;
  Uint32 ticks = clock.getElapsedTicks();
  FrameGenerator frameGen;

  while ( !done ) {
    // The next loop polls for events, guarding against key bounce:
    while ( SDL_PollEvent(&event) ) {
      keystate = SDL_GetKeyboardState(NULL);
      if (event.type ==  SDL_QUIT) { done = true; break; }
      if(event.type == SDL_KEYDOWN) {
        if (keystate[SDL_SCANCODE_ESCAPE] || keystate[SDL_SCANCODE_Q]) {
          done = true;
          break;
        }
        if ( keystate[SDL_SCANCODE_P] ) {
          if ( clock.isPaused() ) clock.unpause();
          else clock.pause();
        }
        if ( keystate[SDL_SCANCODE_T] ) {
          switchSprite();
        }
        if (keystate[SDL_SCANCODE_F4] && !makeVideo) {
          std::cout << "Initiating frame capture" << std::endl;
          makeVideo = true;
        }
        else if (keystate[SDL_SCANCODE_F4] && makeVideo) {
          std::cout << "Terminating frame capture" << std::endl;
          makeVideo = false;
        }
      }
    }

    // In this section of the event loop we allow key bounce:

    ticks = clock.getElapsedTicks();
    if ( ticks > 0 ) {
      clock.incrFrame();
      draw();
      update(ticks);
      if ( makeVideo ) {
        frameGen.makeFrame();
      }
    }
  }
}
