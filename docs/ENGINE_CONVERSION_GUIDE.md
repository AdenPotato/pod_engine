# 🎮 Voxel Engine → Doom Looter-Shooter Conversion Guide

**Project:** Converting existing OpenGL voxel engine to 2D sprite-based Doom-style game  
**Goal:** Build a complete looter-shooter with state machines, loot system, and modern game feel

---

## 📊 Current State vs Target State

### **What You Have (Voxel Engine)**
- ✅ OpenGL context & window management
- ✅ Shader system
- ✅ Texture loading
- ✅ Basic rendering pipeline
- ✅ Voxel rendering (chunk system, meshing, etc.)
- ✅ Camera system (likely 3D FPS-style)
- ✅ Input handling

### **What You Need (Doom-Style Game)**
- ❌ 2D sprite rendering system
- ❌ Sprite rotation/billboarding system
- ❌ State machine executor
- ❌ Weapon system with animations
- ❌ Enemy AI framework
- ❌ Collision detection (2D AABB or ray-based)
- ❌ Audio system
- ❌ Particle effects
- ❌ UI/HUD system
- ❌ Loot generation & affix system
- ❌ Inventory system (Tetris-style grid)
- ❌ Resource/asset management
- ❌ Scene/level management

---

## 🎯 Phase 1: Core Rendering Transition

### **1.1 Sprite Rendering System (~300 lines)**

Replace voxel rendering with 2D sprite rendering.

```cpp
// SpriteRenderer.h
#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Texture;
class Shader;

class SpriteRenderer {
public:
    SpriteRenderer(Shader* shader);
    ~SpriteRenderer();
    
    // Draw single sprite
    void DrawSprite(
        Texture* texture,
        glm::vec2 position,
        glm::vec2 size = glm::vec2(32, 32),
        float rotation = 0.0f,
        glm::vec4 color = glm::vec4(1.0f)
    );
    
    // Draw sprite with UV coordinates (for sprite sheets)
    void DrawSpriteUV(
        Texture* texture,
        glm::vec2 position,
        glm::vec2 size,
        glm::vec4 uvRect,  // (u, v, width, height)
        float rotation,
        glm::vec4 color
    );
    
    // Batch rendering (optional, for performance)
    void BeginBatch();
    void AddSprite(/* params */);
    void EndBatch();
    
private:
    void InitRenderData();
    
    Shader* shader;
    GLuint VAO, VBO;
};
```

**Key Implementation Details:**
```cpp
// SpriteRenderer.cpp
void SpriteRenderer::InitRenderData() {
    // Configure VAO/VBO for a quad
    GLuint VBO;
    float vertices[] = {
        // pos      // tex
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,
        
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glBindVertexArray(VAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void SpriteRenderer::DrawSprite(Texture* texture, glm::vec2 position, 
                                  glm::vec2 size, float rotation, glm::vec4 color) {
    shader->Use();
    
    // Model matrix
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position, 0.0f));
    model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
    model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));
    model = glm::scale(model, glm::vec3(size, 1.0f));
    
    shader->SetMatrix4("model", model);
    shader->SetVector4f("spriteColor", color);
    
    glActiveTexture(GL_TEXTURE0);
    texture->Bind();
    
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}
```

**Sprite Shader:**
```glsl
// sprite.vert
#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 position, vec2 texCoords>

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 projection;

void main() {
    TexCoords = vertex.zw;
    gl_Position = projection * model * vec4(vertex.xy, 0.0, 1.0);
}

// sprite.frag
#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D image;
uniform vec4 spriteColor;

void main() {
    color = spriteColor * texture(image, TexCoords);
}
```

---

### **1.2 Camera Transition (2D Orthographic) (~100 lines)**

Convert 3D perspective camera to 2D orthographic.

```cpp
// Camera2D.h
#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera2D {
public:
    Camera2D(float viewportWidth, float viewportHeight);
    
    // Camera control
    void SetPosition(glm::vec2 position);
    void Move(glm::vec2 offset);
    void SetZoom(float zoom);
    
    // Getters
    glm::mat4 GetProjectionMatrix() const;
    glm::mat4 GetViewMatrix() const;
    glm::vec2 GetPosition() const { return position; }
    float GetZoom() const { return zoom; }
    
    // Screen-to-world conversion
    glm::vec2 ScreenToWorld(glm::vec2 screenPos) const;
    
private:
    void UpdateMatrices();
    
    glm::vec2 position;
    float zoom;
    float viewportWidth;
    float viewportHeight;
    
    glm::mat4 projection;
    glm::mat4 view;
};
```

**Implementation:**
```cpp
Camera2D::Camera2D(float width, float height) 
    : position(0, 0), zoom(1.0f), viewportWidth(width), viewportHeight(height) {
    UpdateMatrices();
}

void Camera2D::UpdateMatrices() {
    // Orthographic projection
    float halfWidth = viewportWidth / (2.0f * zoom);
    float halfHeight = viewportHeight / (2.0f * zoom);
    
    projection = glm::ortho(
        -halfWidth, halfWidth,
        -halfHeight, halfHeight,
        -1.0f, 1.0f
    );
    
    // View matrix (camera transform)
    view = glm::mat4(1.0f);
    view = glm::translate(view, glm::vec3(-position, 0.0f));
}

glm::vec2 Camera2D::ScreenToWorld(glm::vec2 screenPos) const {
    // Convert screen coordinates to world coordinates
    glm::vec2 normalized = glm::vec2(
        (screenPos.x / viewportWidth) * 2.0f - 1.0f,
        1.0f - (screenPos.y / viewportHeight) * 2.0f
    );
    
    glm::vec4 worldPos = glm::inverse(projection * view) * glm::vec4(normalized, 0.0f, 1.0f);
    return glm::vec2(worldPos);
}
```

---

### **1.3 Sprite Atlas/Animation System (~200 lines)**

Handle sprite sheets for animated characters and weapons.

```cpp
// SpriteAtlas.h
#pragma once
#include <unordered_map>
#include <string>
#include <glm/glm.hpp>

class Texture;

struct SpriteFrame {
    glm::vec4 uvRect;  // (u, v, width, height) in 0-1 range
    glm::vec2 size;    // Actual pixel size
    glm::vec2 offset;  // Drawing offset
};

class SpriteAtlas {
public:
    SpriteAtlas(Texture* atlasTexture, int frameWidth, int frameHeight);
    
    // Get frame by name (e.g., "PISGA0")
    SpriteFrame GetFrame(const std::string& frameName) const;
    
    // Get frame by grid position
    SpriteFrame GetFrame(int x, int y) const;
    
    // Add named frame
    void AddFrame(const std::string& name, int x, int y);
    
    Texture* GetTexture() const { return texture; }
    
private:
    Texture* texture;
    int frameWidth;
    int frameHeight;
    int atlasWidth;
    int atlasHeight;
    
    std::unordered_map<std::string, SpriteFrame> frames;
};

// Usage:
// SpriteAtlas* pistolAtlas = new SpriteAtlas(pistolTexture, 64, 64);
// pistolAtlas->AddFrame("PISGA0", 0, 0);  // First frame
// pistolAtlas->AddFrame("PISGB0", 1, 0);  // Second frame
```

---

## 🎯 Phase 2: State Machine System

### **2.1 State Machine Parser (~400 lines)**

Parse `.states` files into executable state machines.

```cpp
// StateMachine.h
#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>

struct ActionCall {
    std::string name;
    std::vector<std::string> parameters;
};

enum class TransitionType {
    None,
    Goto,
    Loop,
    Stop
};

struct Transition {
    TransitionType type;
    std::string targetState;
    int offset;  // For Goto State+N
};

struct StateLine {
    std::string sprite;
    int duration;  // -1 = infinite, 0 = instant, >0 = tics
    std::vector<ActionCall> actions;
    std::vector<std::string> flags;  // e.g., "Bright"
    Transition transition;
};

struct StateBlock {
    std::string name;
    std::vector<StateLine> lines;
};

class StateMachine {
public:
    StateMachine() = default;
    
    // Parse from file
    bool LoadFromFile(const std::string& filepath);
    
    // Get state
    const StateBlock* GetState(const std::string& name) const;
    bool HasState(const std::string& name) const;
    
    const std::unordered_map<std::string, StateBlock>& GetStates() const { return states; }
    
private:
    bool ParseStateFile(const std::string& content);
    StateLine ParseLine(const std::string& line);
    ActionCall ParseAction(const std::string& actionStr);
    
    std::unordered_map<std::string, StateBlock> states;
};
```

**Parser Implementation:**
```cpp
// StateMachine.cpp
bool StateMachine::LoadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open state file: " << filepath << "\n";
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return ParseStateFile(buffer.str());
}

bool StateMachine::ParseStateFile(const std::string& content) {
    // Split into state blocks
    std::string currentStateName;
    std::vector<std::string> currentLines;
    
    std::istringstream stream(content);
    std::string line;
    
    bool inState = false;
    
    while (std::getline(stream, line)) {
        // Trim whitespace
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '/' && line[1] == '/') continue;
        
        // Check for state name
        if (line.find('{') != std::string::npos) {
            currentStateName = line.substr(0, line.find('{'));
            // Trim state name
            currentStateName.erase(currentStateName.find_last_not_of(" \t") + 1);
            inState = true;
            currentLines.clear();
        }
        else if (line.find('}') != std::string::npos) {
            // End of state block
            if (inState) {
                StateBlock block;
                block.name = currentStateName;
                
                for (const auto& stateLine : currentLines) {
                    block.lines.push_back(ParseLine(stateLine));
                }
                
                states[currentStateName] = block;
                inState = false;
            }
        }
        else if (inState) {
            currentLines.push_back(line);
        }
    }
    
    return true;
}

StateLine StateMachine::ParseLine(const std::string& line) {
    StateLine result;
    result.duration = 1;  // Default
    result.transition.type = TransitionType::None;
    
    // Check for special commands
    if (line == "Loop") {
        result.transition.type = TransitionType::Loop;
        return result;
    }
    if (line == "Stop") {
        result.transition.type = TransitionType::Stop;
        return result;
    }
    if (line.substr(0, 4) == "Goto") {
        result.transition.type = TransitionType::Goto;
        std::string target = line.substr(5);
        
        // Check for offset (e.g., "Goto Reload+1")
        size_t plusPos = target.find('+');
        if (plusPos != std::string::npos) {
            result.transition.targetState = target.substr(0, plusPos);
            result.transition.offset = std::stoi(target.substr(plusPos + 1));
        } else {
            result.transition.targetState = target;
            result.transition.offset = 0;
        }
        return result;
    }
    
    // Parse normal line: SPRITE [, DURATION] [, ACTIONS...] [, FLAGS]
    std::vector<std::string> tokens;
    std::stringstream ss(line);
    std::string token;
    
    while (std::getline(ss, token, ',')) {
        // Trim whitespace
        token.erase(0, token.find_first_not_of(" \t"));
        token.erase(token.find_last_not_of(" \t") + 1);
        tokens.push_back(token);
    }
    
    if (tokens.empty()) return result;
    
    // First token is always sprite
    result.sprite = tokens[0];
    
    // Parse remaining tokens
    for (size_t i = 1; i < tokens.size(); i++) {
        const std::string& tok = tokens[i];
        
        // Check if it's a number (duration)
        if (std::isdigit(tok[0]) || tok[0] == '-') {
            result.duration = std::stoi(tok);
        }
        // Check if it's an action (contains parentheses)
        else if (tok.find('(') != std::string::npos) {
            result.actions.push_back(ParseAction(tok));
        }
        // Check for flags
        else if (tok == "Bright") {
            result.flags.push_back(tok);
        }
        // Otherwise, it's an action without parameters
        else {
            ActionCall action;
            action.name = tok;
            result.actions.push_back(action);
        }
    }
    
    return result;
}

ActionCall StateMachine::ParseAction(const std::string& actionStr) {
    ActionCall action;
    
    size_t parenPos = actionStr.find('(');
    action.name = actionStr.substr(0, parenPos);
    
    // Extract parameters
    size_t endParen = actionStr.find(')');
    if (parenPos != std::string::npos && endParen != std::string::npos) {
        std::string params = actionStr.substr(parenPos + 1, endParen - parenPos - 1);
        
        // Split by comma
        std::stringstream ss(params);
        std::string param;
        while (std::getline(ss, param, ',')) {
            param.erase(0, param.find_first_not_of(" \t"));
            param.erase(param.find_last_not_of(" \t") + 1);
            if (!param.empty()) {
                action.parameters.push_back(param);
            }
        }
    }
    
    return action;
}
```

---

### **2.2 State Machine Executor (~300 lines)**

Execute state machines at 35 Hz.

```cpp
// StateMachineComponent.h
#pragma once
#include "StateMachine.h"
#include <string>

class Entity;

class StateMachineComponent {
public:
    StateMachineComponent(Entity* owner, StateMachine* machine);
    
    // State control
    void PlayState(const std::string& stateName);
    void Tick();  // Called at 35 Hz
    
    // Getters
    std::string GetCurrentState() const { return currentState; }
    std::string GetCurrentSprite() const;
    bool IsStatePlaying(const std::string& state) const;
    
private:
    void ExecuteActions(const std::vector<ActionCall>& actions);
    void HandleTransition(const Transition& transition);
    
    Entity* owner;
    StateMachine* machine;
    
    std::string currentState;
    int frameIndex;
    int tickCounter;
};
```

**Executor Implementation:**
```cpp
void StateMachineComponent::Tick() {
    if (currentState.empty()) return;
    
    const StateBlock* state = machine->GetState(currentState);
    if (!state || state->lines.empty()) return;
    
    if (frameIndex >= state->lines.size()) return;
    
    const StateLine& line = state->lines[frameIndex];
    
    // Increment tick counter
    tickCounter++;
    
    // Check if we should advance to next frame
    if (line.duration == -1) {
        // Hold forever
        return;
    }
    else if (line.duration == 0 || tickCounter >= line.duration) {
        // Execute actions
        ExecuteActions(line.actions);
        
        // Handle transition
        if (line.transition.type != TransitionType::None) {
            HandleTransition(line.transition);
        } else {
            // Advance to next frame
            frameIndex++;
            tickCounter = 0;
            
            // Check if we reached end of state
            if (frameIndex >= state->lines.size()) {
                frameIndex = state->lines.size() - 1;  // Stay on last frame
            }
        }
    }
}

void StateMachineComponent::HandleTransition(const Transition& transition) {
    switch (transition.type) {
        case TransitionType::Loop:
            frameIndex = 0;
            tickCounter = 0;
            break;
            
        case TransitionType::Stop:
            // Stay on current frame
            break;
            
        case TransitionType::Goto:
            PlayState(transition.targetState);
            frameIndex = transition.offset;
            tickCounter = 0;
            break;
            
        default:
            break;
    }
}

void StateMachineComponent::ExecuteActions(const std::vector<ActionCall>& actions) {
    for (const auto& action : actions) {
        // This is where you call your action functions
        // See Phase 3 for Action System implementation
        ActionExecutor::Execute(owner, action);
    }
}
```

---

## 🎯 Phase 3: Action System

### **3.1 Action Executor (~500 lines)**

Implement all weapon/enemy actions.

```cpp
// ActionExecutor.h
#pragma once
#include <string>
#include <functional>
#include <unordered_map>

class Entity;
struct ActionCall;

using ActionFunction = std::function<void(Entity*, const std::vector<std::string>&)>;

class ActionExecutor {
public:
    static void Init();
    static void Execute(Entity* entity, const ActionCall& action);
    static void RegisterAction(const std::string& name, ActionFunction func);
    
private:
    static std::unordered_map<std::string, ActionFunction> actions;
};

// Core Actions
namespace Actions {
    void WeaponReady(Entity* entity, const std::vector<std::string>& params);
    void Raise(Entity* entity, const std::vector<std::string>& params);
    void Lower(Entity* entity, const std::vector<std::string>& params);
    void GunFlash(Entity* entity, const std::vector<std::string>& params);
    void FireBullets(Entity* entity, const std::vector<std::string>& params);
    void FireProjectile(Entity* entity, const std::vector<std::string>& params);
    void ReFire(Entity* entity, const std::vector<std::string>& params);
    void CheckReload(Entity* entity, const std::vector<std::string>& params);
    void ReloadComplete(Entity* entity, const std::vector<std::string>& params);
    void PlaySound(Entity* entity, const std::vector<std::string>& params);
    void Light(Entity* entity, const std::vector<std::string>& params);
    void ScreenShake(Entity* entity, const std::vector<std::string>& params);
}
```

**Example Action Implementations:**
```cpp
// ActionExecutor.cpp
void ActionExecutor::Init() {
    RegisterAction("WeaponReady", Actions::WeaponReady);
    RegisterAction("Raise", Actions::Raise);
    RegisterAction("Lower", Actions::Lower);
    RegisterAction("GunFlash", Actions::GunFlash);
    RegisterAction("FireBullets", Actions::FireBullets);
    RegisterAction("FireProjectile", Actions::FireProjectile);
    RegisterAction("ReFire", Actions::ReFire);
    RegisterAction("CheckReload", Actions::CheckReload);
    RegisterAction("ReloadComplete", Actions::ReloadComplete);
    RegisterAction("PlaySound", Actions::PlaySound);
    RegisterAction("Light", Actions::Light);
    RegisterAction("ScreenShake", Actions::ScreenShake);
}

void ActionExecutor::Execute(Entity* entity, const ActionCall& action) {
    auto it = actions.find(action.name);
    if (it != actions.end()) {
        it->second(entity, action.parameters);
    } else {
        std::cerr << "Unknown action: " << action.name << "\n";
    }
}

// Example implementations
namespace Actions {
    void FireBullets(Entity* entity, const std::vector<std::string>& params) {
        // Get weapon component
        WeaponComponent* weapon = entity->GetComponent<WeaponComponent>();
        if (!weapon) return;
        
        // Parse parameters (or use weapon defaults)
        int count = params.size() > 0 ? std::stoi(params[0]) : 1;
        float spreadH = params.size() > 1 ? std::stof(params[1]) : weapon->spread.x;
        float spreadV = params.size() > 2 ? std::stof(params[2]) : weapon->spread.y;
        int damage = params.size() > 3 ? std::stoi(params[3]) : weapon->damage;
        
        // Get player entity to determine fire origin
        Entity* player = EntityManager::GetPlayer();
        glm::vec2 position = player->position;
        float angle = player->GetAimAngle();
        
        // Fire bullets
        for (int i = 0; i < count; i++) {
            // Add random spread
            float spreadAngle = angle + 
                (((rand() / (float)RAND_MAX) - 0.5f) * 2.0f * spreadH);
            
            glm::vec2 direction(cos(spreadAngle), sin(spreadAngle));
            
            // Raycast
            RaycastHit hit = PhysicsWorld::Instance().Raycast(
                position, direction, weapon->range
            );
            
            if (hit.hit) {
                // Damage enemy
                Entity* hitEntity = (Entity*)hit.entity;
                if (hitEntity->HasComponent<HealthComponent>()) {
                    hitEntity->GetComponent<HealthComponent>()->TakeDamage(damage);
                }
                
                // Spawn impact effect
                ParticleManager::SpawnEffect("BulletPuff", hit.point);
            }
        }
        
        // Consume ammo
        weapon->currentAmmo--;
    }
    
    void PlaySound(Entity* entity, const std::vector<std::string>& params) {
        if (params.empty()) return;
        
        std::string soundKey = params[0];
        float volume = params.size() > 1 ? std::stof(params[1]) : 1.0f;
        
        // Get sound name from weapon definition
        WeaponComponent* weapon = entity->GetComponent<WeaponComponent>();
        if (weapon && weapon->definition) {
            auto it = weapon->definition->sounds.find(soundKey);
            if (it != weapon->definition->sounds.end()) {
                AudioSystem::Instance().PlaySound(it->second, volume);
            }
        }
    }
    
    void CheckReload(Entity* entity, const std::vector<std::string>& params) {
        WeaponComponent* weapon = entity->GetComponent<WeaponComponent>();
        if (!weapon) return;
        
        int threshold = params.size() > 0 ? std::stoi(params[0]) : 0;
        
        if (weapon->currentAmmo <= threshold) {
            // Jump to Reload state
            StateMachineComponent* sm = entity->GetComponent<StateMachineComponent>();
            if (sm) {
                sm->PlayState("Reload");
            }
        }
    }
}
```

---

## 🎯 Phase 4: Game Systems

### **4.1 Entity/Component System (~400 lines)**

Already described in previous conversation - see Entity.h example.

### **4.2 Weapon System (~300 lines)**

```cpp
// WeaponComponent.h
#pragma once
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>

class WeaponDefinition;

class WeaponComponent {
public:
    WeaponComponent(WeaponDefinition* def);
    
    void Fire();
    void Reload();
    void SwitchTo();
    void SwitchAway();
    
    // State
    WeaponDefinition* definition;
    int currentAmmo;
    int reserveAmmo;
    
    // Computed stats (base + affixes)
    int damage;
    glm::vec2 spread;
    float range;
    float fireRate;
    
    // Affixes (for loot system)
    std::vector<Affix> affixes;
    Rarity rarity;
};

// WeaponDefinition (loaded from data files)
class WeaponDefinition {
public:
    std::string name;
    WeaponType type;  // Kinetic, Special, Heavy
    
    // Base stats
    int baseDamage;
    glm::vec2 baseSpread;
    float baseRange;
    int magazineSize;
    int reserveMax;
    
    // Visual
    std::string spriteAtlas;
    glm::vec2 inventorySize;
    
    // Audio
    std::unordered_map<std::string, std::string> sounds;
    
    // State machine
    std::string statesFile;
    StateMachine* stateMachine;
};
```

---

### **4.3 Physics System (2D Collision) (~400 lines)**

Already described - see AABB collision and raycasting from previous message.

### **4.4 Audio System (~300 lines)**

Already described - see OpenAL implementation from previous message.

### **4.5 Particle System (~300 lines)**

Already described - CPU particles are sufficient to start.

---

## 🎯 Phase 5: Loot & Inventory

### **5.1 Affix System (~300 lines)**

```cpp
// AffixSystem.h
#pragma once
#include <vector>
#include <string>

enum class AffixType {
    IncreasedDamage,
    IncreasedFireRate,
    IncreasedMagazineSize,
    IncreasedAccuracy,
    // ... etc
};

enum class Rarity {
    Common,     // 0-1 affixes
    Uncommon,   // 1-2 affixes
    Rare,       // 2-4 affixes
    Legendary,  // 3-6 affixes
    Exotic      // Fixed unique affixes
};

struct Affix {
    AffixType type;
    float value;
};

class AffixGenerator {
public:
    static std::vector<Affix> RollAffixes(Rarity rarity, int maxSlots);
    static void ApplyAffixes(WeaponComponent* weapon, const std::vector<Affix>& affixes);
};
```

---

### **5.2 Inventory System (Tetris Grid) (~500 lines)**

```cpp
// InventorySystem.h
#pragma once
#include <glm/glm.hpp>
#include <vector>

struct InventoryItem {
    Entity* entity;
    glm::vec2 position;  // Grid position
    glm::vec2 size;      // Grid size
};

class Inventory {
public:
    Inventory(int gridWidth, int gridHeight);
    
    // Item management
    bool AddItem(Entity* item, glm::vec2 size);
    bool AddItemAt(Entity* item, glm::vec2 position, glm::vec2 size);
    void RemoveItem(Entity* item);
    bool MoveItem(Entity* item, glm::vec2 newPosition);
    
    // Queries
    bool CanFitAt(glm::vec2 position, glm::vec2 size) const;
    Entity* GetItemAt(glm::vec2 position) const;
    
    // Equipped weapons
    Entity* equippedWeapon1;
    Entity* equippedWeapon2;
    int activeWeapon;  // 1 or 2
    
private:
    int gridWidth;
    int gridHeight;
    std::vector<std::vector<bool>> grid;  // Occupancy grid
    std::vector<InventoryItem> items;
};
```

---

## 🎯 Phase 6: UI & Polish

### **6.1 Text Rendering (~200 lines)**

Using FreeType or stb_truetype.

```cpp
// TextRenderer.h
#pragma once
#include <glm/glm.hpp>
#include <string>

class TextRenderer {
public:
    void Init(const std::string& fontPath, int fontSize);
    void RenderText(const std::string& text, glm::vec2 position, 
                    float scale, glm::vec3 color);
    glm::vec2 MeasureText(const std::string& text, float scale);
};
```

---

### **6.2 HUD System (~300 lines)**

```cpp
// HUD.h
#pragma once
class HUD {
public:
    void Render();
    
private:
    void RenderHealthBar();
    void RenderAmmoCounter();
    void RenderWeaponDisplay();
    void RenderCrosshair();
};
```

---

### **6.3 Resource Manager (~250 lines)**

Already described - see ResourceManager from previous message.

---

## 📊 Complete System Overview

### **Systems Priority List**

| Priority | System | Lines | Dependency |
|----------|--------|-------|------------|
| 🔴 **Critical** | Sprite Renderer | 300 | None |
| 🔴 **Critical** | Camera 2D | 100 | None |
| 🔴 **Critical** | Sprite Atlas | 200 | Sprite Renderer |
| 🔴 **Critical** | State Parser | 400 | None |
| 🔴 **Critical** | State Executor | 300 | State Parser |
| 🔴 **Critical** | Action System | 500 | State Executor |
| 🟡 **Important** | Entity System | 400 | None |
| 🟡 **Important** | Weapon System | 300 | Entity, Actions |
| 🟡 **Important** | Physics 2D | 400 | None |
| 🟡 **Important** | Audio System | 300 | None |
| 🟡 **Important** | Particle System | 300 | Sprite Renderer |
| 🟢 **Nice to Have** | Affix System | 300 | Weapon System |
| 🟢 **Nice to Have** | Inventory | 500 | Entity System |
| 🟢 **Nice to Have** | Text Renderer | 200 | None |
| 🟢 **Nice to Have** | HUD System | 300 | Text Renderer |
| 🟢 **Nice to Have** | Resource Mgr | 250 | None |

**Total New Code: ~5,200 lines**

---

## 🗺️ What to Do With Voxel Code

### **Option 1: Keep It (Hybrid Approach)**

Use voxels for level geometry, sprites for entities:
- ✅ Destructible environments (like Teardown)
- ✅ 3D depth for parallax
- ✅ Voxel-based level editor

**Implementation:**
- Render voxel world as background layer
- Render sprites on top with depth sorting
- Collision against voxel grid

**Example:**
```cpp
void Render() {
    // Render voxel world
    voxelRenderer.Render(camera);
    
    // Render sprites (entities, enemies, items)
    for (Entity* entity : entities) {
        spriteRenderer.DrawSprite(
            entity->sprite,
            entity->position,
            entity->size
        );
    }
}
```

---

### **Option 2: Remove It (Pure 2D)**

Strip out all voxel code, go full 2D:
- ✅ Simpler codebase
- ✅ Better performance
- ✅ Easier to work with

**Keep from voxel engine:**
- Window/input management
- Shader system
- Texture loading
- Basic OpenGL setup

**Remove:**
- Chunk system
- Voxel meshing
- 3D camera
- Octree/spatial structures (if any)

---

### **Option 3: Separate Branches**

- `main` = 2D Doom game
- `voxel` = Experimental voxel branch

Later, you can merge features if voxels prove useful.

---

## 📦 Recommended Libraries

All of these you probably already have or are header-only:

| System | Library | Why |
|--------|---------|-----|
| **Windowing** | GLFW | You likely have this |
| **OpenGL Loading** | GLAD | You likely have this |
| **Math** | GLM | Header-only, essential |
| **Image Loading** | stb_image.h | Single header |
| **Audio** | OpenAL Soft | Industry standard |
| **Audio Files** | stb_vorbis.c | Single header, OGG support |
| **Text** | stb_truetype.h | Single header |
| **JSON** | nlohmann/json | Header-only (for data files) |

---

## 🚀 Final Recommendation

**Start with Option 2: Pure 2D**

1. **Fork your voxel engine** (keep it safe)
2. **Strip out voxel code** (clean slate)
3. **Follow the phase plan** (rendering foundation)
4. **Build incrementally** (test at each step)

**Why pure 2D first:**
- ✅ Faster iteration
- ✅ Simpler debugging
- ✅ Can always add voxels later
- ✅ Focus on gameplay, not tech

**You can revisit voxels later** for:
- Destructible cover
- Dynamic level deformation
- Unique visual style

But get the core game working first!

---

## 💡 Pro Tips

1. **Don't optimize early** - Get it working first
2. **Test incrementally** - Every feature = test immediately
3. **Keep voxel backup** - Never delete, just branch
4. **Focus on feel** - State machines are all about game feel
5. **Start simple** - One weapon, one enemy, one level

**Most important:** Have fun! This is a cool project. 🎮

---

**Ready to start building!** 🚀
