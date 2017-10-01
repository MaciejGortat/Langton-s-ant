#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <map>
#include <vector>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <fstream>
#include <cmath>
using namespace std;

enum direction {
	TOP = 0, LEFT = 1, DOWN = 2, RIGHT = 3
};

class Ant {
private:
	Ant() {}
	
public:
	SDL_Point 		pos;
	int 			d; // direction
	int 			offset;
	Ant(int x, int y, int o) {
		pos.x 	= x;
		pos.y 	= y;
		offset 	= o;
		d 		= 0;
	}
	~Ant() {}
	void move(bool* field) {
		*field ? ++d : --d;
		if (d < 0) d = 3;
		if (d > 3) d = 0;
		switch (d) {
			case (0):
				pos.y -= offset;
				break;
			case (1):
				pos.x -= offset;
				break;
			case (2):
				pos.y += offset;
				break;
			case (3):
				pos.x += offset;
				break;
		}
		*field = !(*field);
	}
};

class SDL {
	private:
		SDL_Event					event;
		unsigned short				fps;
		unsigned int				fpsTimer;
		
		SDL() : dbg("error.txt", ios::trunc | ios::out) {
			// Inicjalizacja SDL'a
			if (SDL_Init(SDL_INIT_EVERYTHING)) {
				isError = true;
				return;
			}
			// inicjalizacja okna
			//-------------------------------------------------
			window = SDL_CreateWindow(
				"mruwa",
				SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
				windowWidth, windowHeight,
				SDL_WINDOW_SHOWN | SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_MOUSE_FOCUS
			);
			//-------------------------------------------------
			if (window == nullptr) {
				isError = true;
				return;
			}
			// inicjalizacja renderera
			renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
			if (renderer == nullptr) {
				isError = true;
				return;
			}
			// pobranie tickow
			fpsTimer = SDL_GetTicks();
		}

		~SDL() {
			ants.clear();
			dbg.close();
			SDL_DestroyRenderer(renderer);
			SDL_DestroyWindow(window);
			IMG_Quit();
			SDL_Quit();
		}

	public:
		static SDL& init() {
			static SDL sdl;
			return sdl;
		}

		bool						isError 		= false;
		SDL_Window*					window			= nullptr;
		SDL_Renderer*				renderer 		= nullptr;
		const static int 			windowWidth 	= 600;
		const static int 			windowHeight 	= 600;
		bool 						screen[windowWidth][windowHeight] = {};
		int 						flag = 0;
		ofstream 					dbg;
		vector<Ant*> 				ants;
		void showError() {
			cout << "Error: " << SDL_GetError() << endl;
		}

		SDL_Event* eventUpdate() {
			SDL_PollEvent(&event);
			return &event;
		}
		// odswiezanie
		void screenUpdate(SDL_Event* event) {
			SDL_SetRenderDrawColor(renderer, 0, 60, 0, SDL_ALPHA_OPAQUE);
			if (ants.size() > 0) {
					updateAnts();
			}
			if (SDL_GetTicks() - fpsTimer > fps) {
				
				SDL_RenderClear(renderer);
				for (int x = 0; x < windowWidth; ++x) {
					for (int y = 0; y < windowHeight; ++y) {

						if (screen[x][y]) {
							SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
							SDL_RenderDrawPoint(renderer, x, y);
						}
					}
				}
			}
			
			SDL_RenderPresent(renderer);
			fpsTimer = SDL_GetTicks();
		}
		void updateAnts(void) {
			for (int i = 0; i < ants.size(); ++i) {
				if (ants[i]->pos.x <= 0 || ants[i]->pos.x >= windowWidth || ants[i]->pos.y <= 0 || ants[i]->pos.y >= windowHeight) {
					delete ants[i];
					ants.erase(ants.begin() + i);
				}
				ants[i]->move(&screen[ants[i]->pos.x][ants[i]->pos.y]);
			}
		}
		void pushAnt(int x, int y, int o) {
			Ant* newAnt = new Ant(x, y, o);
			ants.push_back(newAnt);
		}
		void killAnts(void) {
			ants.erase(ants.begin(), ants.end());
			for (int x = 0; x < windowWidth; ++x) {
				for (int y = 0; y < windowHeight; ++y) {
					screen[x][y] = 0;
				}
			}
		}

		void setFPS(unsigned int newFPS) {
			fps =  1.0 / newFPS;
		}
};

int main(int argc, char** argv) {
	SDL& sdl = SDL::init();
	sdl.setFPS(30);
	
	for (SDL_Event* event = sdl.eventUpdate(); event->type != SDL_QUIT; sdl.eventUpdate()) {
		sdl.screenUpdate(event);
		SDL_Delay(1);
		//sdl.updateAnts();
		//Event handling
		switch (event->type) {
			case (SDL_MOUSEBUTTONDOWN): {
				if (sdl.flag) {
					sdl.screen[event->button.x][event->button.y] = 1;
					sdl.pushAnt(event->button.x, event->button.y, 1);
					//sdl.dbg << event->button.x << " " << event->button.y << endl;
					sdl.flag = 0;
				}
						
				break;
			}
			case (SDL_KEYUP): {
				if (event->key.keysym.scancode == SDL_SCANCODE_R) {
					sdl.killAnts();
				}
				break;
			}
			default: {
				sdl.flag = 1;
				break;
			}
		}
	}
	return 0;
}