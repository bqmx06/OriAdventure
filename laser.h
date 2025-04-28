#ifndef LASER_H
#define LASER_H

#include <SDL.h>
#include <cmath>
struct Fireball {
    float x, y;                     // Vị trí xuất hiện
    int width = 249, height = 216;     // Kích thước 1 frame
    int currentFrame = 0;            // Frame hiện tại
    int frameTimer = 0;              // Đếm thời gian đổi frame

    AnimationData animations[2] = {
        {9, 0, 20}, // 9 frame, nằm ở row 0, 5 ticks đổi 1 frame
        {5,1,20}
    };

    SDL_Texture* texture = nullptr;
    SDL_RendererFlip flipType = SDL_FLIP_NONE;

    // Constructor khởi tạo
    Fireball()
        : x(0), y(0)
    {
        // texture sẽ gán từ ngoài
    }
    ~Fireball(){
        if (texture) {
            SDL_DestroyTexture(texture);
        }
    }
    // Update animation
    void update() {
        frameTimer++;
        AnimationData& anim = animations[0];
        if (frameTimer >= anim.frameSpeed) {
            frameTimer = 0;
            if (currentFrame < anim.frameCount - 1) {
                currentFrame++; // Cứ charge dần cho tới frame cuối
            }
            else 
            {anim=animations[1];
            currentFrame=0;}
            // Nếu muốn loop idle sau charge, thêm logic ở đây.
        }
    }

    // Render fireball
    void render(SDL_Renderer* renderer, int offsetX = 0, int offsetY = 0) {
        if (!texture) return;

        const AnimationData& anim = animations[0];
        SDL_Rect clip = {
            currentFrame * width,
            anim.row * height,
            width, height
        };
        SDL_Rect destRect = { (int)x + offsetX, (int)y + offsetY, width, height };

        SDL_RenderCopyEx(renderer, texture, &clip, &destRect, 0, NULL, flipType);

        // Debug vẽ khung:
        // SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        // SDL_RenderDrawRect(renderer, &destRect);
    }
};
struct Laser {
    float startX, startY;
    float endX,   endY;
    int   length;       // chiều dài beam
    float angle;        // hướng beam (độ)
    Uint32 spawnTime;   // ms tạo
    Uint32 lifetime;    // ms tồn tại
    Uint32 fadeDuration;
    bool   isActive;
    SDL_Texture* texture;
    Uint8 alpha;
    SDL_Rect collisionBox;

    Laser(float sX, float sY, float eX, float eY,
          SDL_Texture* tex, Uint32 life_ms = 2000,Uint32 fade_ms=500)
      : startX(sX), startY(sY)
      , endX(eX), endY(eY)
      , spawnTime(SDL_GetTicks())
      , fadeDuration(fade_ms)
      , lifetime(life_ms)
      , isActive(true)
      , texture(tex)
      , alpha(0)
    {
        // tính chiều dài và góc
        float dx = endX - startX;
        float dy = endY - startY;
        // tính chiều dài bằng công thức sqrt(dx² + dy²)
        length = static_cast<int>(std::sqrt(dx*dx + dy*dy));

        // lấy góc bằng atan2 (trả về double), rồi quy về float độ
        angle = static_cast<float>(std::atan2(dy, dx) * 180.0 / M_PI);

    }

    void update() {
        if (!isActive) return;
        Uint32 now = SDL_GetTicks();
        Uint32 elapsed = now - spawnTime;
        if (SDL_GetTicks() - spawnTime > lifetime)
            isActive = false;
            // fade-in
        if (elapsed < fadeDuration) {
            alpha = Uint8(255 * (float(elapsed) / fadeDuration));
        }
        // fade-out
        Uint32 remain = lifetime - elapsed;
        if (remain < fadeDuration) {
            alpha = Uint8(255 * (float(remain) / fadeDuration));
        }
        // fully visible
        if (elapsed >= fadeDuration && remain >= fadeDuration) {
            alpha = 255;
        }
    }

    void render(SDL_Renderer* renderer) {
        if (!isActive || !texture) return;

        // Lấy kích thước gốc của texture
        int texW, texH;
        SDL_QueryTexture(texture, nullptr, nullptr, &texW, &texH);

        // Vẽ beam: scale width = length, giữ nguyên height
        SDL_Rect dst = {
            int(startX),
            int(startY - texH/2),  // căn giữa theo trục Y
            length,
            texH
        };

        // Xoay quanh điểm (0, texH/2) để beam xoay đúng
        SDL_Point center = { 0, texH/2 };
        SDL_SetTextureAlphaMod(texture, alpha);
        SDL_RenderCopyEx(renderer,
                         texture,
                         nullptr,
                         &dst,
                         angle,
                         &center,
                         SDL_FLIP_NONE);
        SDL_SetTextureAlphaMod(texture, 255);
    }
};


#endif
