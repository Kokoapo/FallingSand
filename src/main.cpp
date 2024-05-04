#include <SDL.h>
#include <cmath>

// Constantes Globais
// WIDTH = Largura da tela
// HEIGHT = Altura da tela
// SIZE = Tamanho de cada grão de areia
int WIDTH = 800;
int HEIGHT = 600;
int SIZE = 8;

// Realiza o Setup Inicial do SDL, criando window, renderer e texture
void SetupInit(SDL_Window** windowPtr, SDL_Renderer** rendererPtr, SDL_Texture** texturePtr) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        exit(-1);

    Uint32 flags = SDL_WINDOW_SHOWN; 
    *windowPtr = SDL_CreateWindow(
        "FallingSand",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WIDTH,
        HEIGHT,
        flags
    );

    if(!windowPtr) {
        SDL_Quit();
        exit(-1);
    }

    *rendererPtr = SDL_CreateRenderer(*windowPtr, -1, 0);

    if (!*rendererPtr) {
        SDL_DestroyWindow(*windowPtr);
        SDL_Quit();
        exit(-1);
    }

    *texturePtr = SDL_CreateTexture(*rendererPtr, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, WIDTH, HEIGHT);
}

// Limpa todos os dados necessários do SDL (window, renderer e texture) antes de terminar de executar o programa
void EndClear(SDL_Window** windowPtr, SDL_Renderer** rendererPtr, SDL_Texture** texturePtr) {
    SDL_DestroyTexture(*texturePtr);
    SDL_DestroyRenderer(*rendererPtr);
    SDL_DestroyWindow(*windowPtr);
    SDL_Quit();
}

// Desenha pixel à pixel todos os grãos de areia, com suas respectivas cores
void Desenhar(SDL_Renderer** rendererPtr, SDL_Texture** texturePtr, bool **areia, SDL_Color **cores) {
    SDL_SetRenderDrawBlendMode(*rendererPtr, SDL_BLENDMODE_NONE);
    
    SDL_SetRenderTarget(*rendererPtr, *texturePtr);
    SDL_SetRenderDrawBlendMode(*rendererPtr, SDL_BLENDMODE_NONE);
    SDL_SetTextureBlendMode(*texturePtr, SDL_BLENDMODE_NONE);
    SDL_SetRenderDrawColor(*rendererPtr, 0, 0, 0, 0);
    SDL_RenderFillRect(*rendererPtr, NULL);

    for (int i = 0; i < WIDTH/SIZE; i++) {
        for (int j = 0; j < HEIGHT/SIZE; j++) {
            if (areia[i][j]) {
                SDL_SetRenderDrawColor(*rendererPtr, cores[i][j].r, cores[i][j].g, cores[i][j].b, cores[i][j].a);
                SDL_Rect a = { i*SIZE, j*SIZE, SIZE, SIZE };
                SDL_RenderFillRect(*rendererPtr, &a);
            }
        }   
    }
    SDL_SetRenderTarget(*rendererPtr, NULL);

    /* blita textura na areia */
    SDL_SetRenderDrawBlendMode(*rendererPtr, SDL_BLENDMODE_ADD);
    SDL_SetTextureBlendMode(*texturePtr, SDL_BLENDMODE_ADD);
    SDL_RenderCopy(*rendererPtr, *texturePtr, NULL, NULL);
    SDL_SetRenderDrawBlendMode(*rendererPtr, SDL_BLENDMODE_NONE);
}

// Converte o valor em HSV (hue, 1, 1) para um valor em RGBA para ser usado com SDL_Color
SDL_Color HVStoRGB(int16_t hue) {
    float fHPrime = fmod(hue / 60.0, 6);
    float fX = 1 - fabs(fmod(fHPrime, 2) - 1);
    float fR, fG, fB;
  
    if(0 <= fHPrime && fHPrime < 1) {
        fR = 1;
        fG = fX;
        fB = 0;
    } else if(1 <= fHPrime && fHPrime < 2) {
        fR = fX;
        fG = 1;
        fB = 0;
    } else if(2 <= fHPrime && fHPrime < 3) {
        fR = 0;
        fG = 1;
        fB = fX;
    } else if(3 <= fHPrime && fHPrime < 4) {
        fR = 0;
        fG = fX;
        fB = 1;
    } else if(4 <= fHPrime && fHPrime < 5) {
        fR = fX;
        fG = 0;
        fB = 1;
    } else if(5 <= fHPrime && fHPrime < 6) {
        fR = 1;
        fG = 0;
        fB = fX;
    } else {
        fR = 0;
        fG = 0;
        fB = 0;
    }

    return { fR*255, fG*255, fB*255, 255 };
}

// Função Main
int main(int argc, char* argv[]) {
    // Setup Inicial do SDL
    SDL_Window* windowPtr;
    SDL_Renderer* rendererPtr;
    SDL_Texture* texturePtr;
    SetupInit(&windowPtr, &rendererPtr, &texturePtr);

    // Variáveis gerais
    // hue = indica a cor dos próximos grãos de areia
    // frames = controla quantos frames leva para avançar a física dos grãos
    // isDesenhando = controla quando é necessário adicionar novos grãos (desenhar)
    // **areia e **cores = matriz principais que indicam, respectivamente, as posições na tela que possuem grãos e suas respectivas cores
    int16_t hue = 0;
    int8_t frames = 30;
    bool isDesenhando = false;
    bool **areia = new bool*[WIDTH/SIZE];
    SDL_Color **cores = new SDL_Color*[WIDTH/SIZE];
    for (int i = 0; i < WIDTH/SIZE; i++) {
        areia[i] = new bool[HEIGHT/SIZE];
        cores[i] = new SDL_Color[HEIGHT/SIZE];
        for (int j = 0; j < HEIGHT/SIZE; j++) {
            areia[i][j] = false;
            cores[i][j] = { 255, 0, 0, 255 };
        }
    }

    // Game Loop
    bool running = true;
    while (running) {
        // Manejamento (handling) de eventos
        // Caso o botão do mouse seja pressionado: Iniciar estado de desenhar grãos
        // Enquanto no estado de desenhar e houver movimento do mouse: Adicionar novos grãos em (mouse.x,mouse.y) com SDL_Color = hue
        // Caso o botão do mouse seja solto: Sair do estado de desenhar grãos e atualizar hue
        // Caso o SDL encontre um evento de fechamento (SDL_QUIT ou SDL_WINDOWEVENT_CLOSE): Terminar Game Loop
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
            case SDL_MOUSEBUTTONDOWN:
                isDesenhando = true;
                break;
            case SDL_MOUSEMOTION:
                if (isDesenhando) {
                    areia[e.button.x/SIZE][e.button.y/SIZE] = true;
                    cores[e.button.x/SIZE][e.button.y/SIZE] = HVStoRGB(hue);
                }
                break;
            case SDL_MOUSEBUTTONUP:
                hue += 10;
                if (hue > 360)
                    hue = 0;
                isDesenhando = false;
                break;
            case SDL_QUIT:
                running = false;
                break;
            case SDL_WINDOWEVENT:
                if (e.window.event == SDL_WINDOWEVENT_CLOSE && e.window.windowID == SDL_GetWindowID(windowPtr))
                    running = false;
                break;
            default:
                break;
            }
        } 

        // SDL Render para o novo frame 
        SDL_SetRenderDrawColor(rendererPtr, 0, 0, 0, 255);
        SDL_RenderClear(rendererPtr);
        Desenhar(&rendererPtr, &texturePtr, areia, cores);
        SDL_RenderPresent(rendererPtr);

        // Atualiza a física dos grãos a cada X frames 
        frames--;
        if (!frames) {
            // Cria matrizes **areiaProx e **coresProx auxiliares
            // essas matrizes representam o novo estado da tela após aplicar a física
            // são copiadas para as matrizes principais **areia e **cores no final do cálculo
            bool **areiaProx = new bool*[WIDTH/SIZE];
            SDL_Color **coresProx = new SDL_Color*[WIDTH/SIZE];
            for (int i = 0; i < WIDTH/SIZE; i++) {
                areiaProx[i] = new bool[HEIGHT/SIZE];
                coresProx[i] = new SDL_Color[HEIGHT/SIZE];
                for (int j = 0; j < HEIGHT/SIZE; j++) {
                    areiaProx[i][j] = false;
                    coresProx[i][j] = { 255, 0, 0, 255 };
                }
            }

            // Realiza o cálculo da física sobre cada grão
            // Caso o grão esteja no chão: manter posição
            // Senão caso não haja um grão abaixo: deslocar o grão para baixo
            // Senão caso haja um grão abaixo: buscar em volta
            //      Caso não haja um grão à esquerda e abaixo: colocar o grão nessa posição
            //      Senão Caso não haja um grão à direita e abaixo: colocar o grão nessa posição
            //      Senão: manter posição
            for (int i = 0; i < WIDTH/SIZE; i++) {
                for (int j = 0; j < HEIGHT/SIZE; j++) {
                    if (areia[i][j]) {
                        if (j+1 >= HEIGHT/SIZE) {
                            areiaProx[i][j] = true;
                            coresProx[i][j] = cores[i][j];
                        } else if (!areia[i][j+1]) {
                            areiaProx[i][j+1] = true;
                            coresProx[i][j+1] = cores[i][j];
                        } else if (areia[i][j+1]) {
                            if (i-1 >= 0 && !areia[i-1][j+1]) {
                                areiaProx[i-1][j+1] = true;
                                coresProx[i-1][j+1] = cores[i][j];
                            } else if (i+1 < WIDTH/SIZE && !areia[i+1][j+1]) {
                                areiaProx[i+1][j+1] = true;
                                coresProx[i+1][j+1] = cores[i][j];
                            } else {
                                areiaProx[i][j] = true;
                                coresProx[i][j] = cores[i][j];
                            }
                        }
                    }
                }
            }

            // Transfere os valores das matrizes Prox para as principais e libera a memória das matrizes Prox
            for (int i = 0; i < WIDTH/SIZE; i++) {
                for (int j = 0; j < HEIGHT/SIZE; j++) {
                    areia[i][j] = areiaProx[i][j];
                    cores[i][j] = coresProx[i][j];
                }
                delete [] areiaProx[i];
                delete [] coresProx[i];
            }
            delete [] areiaProx;
            delete [] coresProx;

            // Atualiza frames para continuar a contagem
            frames = 30;
        }
    }

    // Liberar memória das matrizes principais e do SDL para fechar o programa
    for (int i = 0; i < WIDTH/SIZE; i++) {
        delete [] areia[i];
        delete [] cores[i];
    }
    delete [] areia;
    delete [] cores;
    EndClear(&windowPtr, &rendererPtr, &texturePtr);
    return 0;
}