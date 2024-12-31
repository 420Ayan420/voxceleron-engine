# Voxceleron Engine

A high-performance Vulkan-based voxel engine focused on efficient rendering and modern graphics techniques. This project aims to create a robust foundation for voxel-based games and simulations using modern graphics APIs.

## Project Overview

The goal of this project is to create a voxel engine that:
1. Uses modern Vulkan techniques for efficient rendering
2. Implements efficient face culling and mesh generation
3. Provides a solid foundation for future voxel-based games
4. Demonstrates best practices in graphics programming

## Technical Implementation

### Core Components

#### Chunk System
- Chunks are 16x16x16 voxel units
- Each chunk manages its own mesh generation
- Chunks are generated and loaded dynamically
- Face culling is implemented at the chunk level to optimize rendering
- Chunk coordinates use integer vectors (glm::ivec3) for precise positioning
- Chunk data structure includes:
  - 3D array of voxels (16x16x16)
  - Vertex data buffer for rendering
  - Position in world space
  - Mesh generation state

#### Voxel System
- Each voxel contains:
  - Type (DIRT, GRASS, STONE, WOOD, LEAVES)
  - Active state flag
  - Color data (RGB)
  - Default colors defined per type
- Voxel faces are only generated when:
  - Adjacent voxel is inactive (air)
  - Adjacent voxel is outside chunk boundaries
  - Face is at world boundary

#### Rendering Pipeline
- Built on Vulkan for modern GPU utilization
- Uses push constants for MVP matrix and lighting information
- Implements depth testing for correct 3D rendering
- Counter-clockwise vertex winding order
- Face culling optimizations at both mesh and render pipeline levels
- Shader Implementation:
  - Vertex shader:
    - Handles MVP transformation
    - Passes color and normal data to fragment shader
  - Fragment shader:
    - Basic directional lighting
    - Normal-based shading
    - Ambient light component
- Render Pass Structure:
  - Color attachment with clear on load
  - Depth attachment for proper 3D rendering
  - ImGui integration in same render pass

#### Memory Management
- Vertex Buffer:
  - Dynamic sizing based on visible faces
  - Host visible for CPU updates
  - Device local for GPU performance
- Chunk Data:
  - Stored in CPU memory for easy updates
  - Transferred to GPU when modified
  - Efficient update system to minimize transfers

### Development Journey & Fixes

#### Initial Setup
1. Basic Vulkan initialization and window creation
2. GLFW integration for window management
3. Basic shader compilation pipeline

#### Major Challenges & Solutions

1. **Mesh Generation**
   - Initial issue: Inefficient vertex generation for each voxel
   - Solution: Implemented face culling during mesh generation
   - Only generates faces that are visible (not adjacent to other voxels)

2. **Face Culling Issues**
   - Problem: Faces were being culled incorrectly, leading to visual artifacts
   - Fix: Separated mesh-level culling from graphics pipeline culling
   - Implemented proper adjacency checks in `shouldRenderFace()`

3. **Rendering Issues**
   - Initial problem: Black screen with no visible geometry
   - Fixes:
     - Added proper depth buffer setup
     - Corrected vertex winding order
     - Implemented proper MVP matrix calculations
     - Fixed shader vertex attribute descriptions

4. **ImGui Integration**
   - Challenge: ImGui rendering conflicting with main render pass
   - Solution: Proper descriptor pool setup and render pass integration
   - Added separate ImGui render pass in the command buffer

5. **Performance Optimization**
   - Implemented efficient vertex buffer updates
   - Added statistics tracking for performance monitoring
   - Optimized chunk updates and mesh generation

#### Additional Technical Challenges

6. **Vertex Buffer Management**
   - Initial Problem: Fixed-size vertex buffers limiting chunk sizes
   - Solution: Dynamic vertex buffer resizing
   - Implementation: Recreate buffers when needed with proper synchronization

7. **Shader Compilation Pipeline**
   - Challenge: Integrating shader compilation into build process
   - Solution: CMake custom commands for shader compilation
   - Added error checking and validation

8. **Memory Synchronization**
   - Issue: Race conditions during chunk updates
   - Fix: Proper fence and semaphore usage
   - Implementation of triple buffering for smooth updates

9. **Coordinate System Transformations**
   - Challenge: Converting between world, chunk, and local coordinates
   - Solution: Implemented helper functions for coordinate transforms
   - Added boundary checking and validation

### Architecture Details

#### World Management
```cpp
class World {
    std::unordered_map<glm::ivec3, std::unique_ptr<Chunk>> chunks;
    // Hash function specialization for glm::ivec3
    // Chunk management functions
    // Coordinate transformation utilities
};
```

#### Chunk Implementation
```cpp
class Chunk {
    static constexpr int CHUNK_SIZE = 16;
    std::array<Voxel, CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE> voxels;
    std::vector<float> vertexData;
    glm::ivec3 position;
    // Mesh generation methods
    // Voxel access methods
};
```

#### Renderer Design
```cpp
class Renderer {
    // Vulkan instance and device management
    // Swapchain and framebuffer handling
    // Command buffer management
    // Shader resource management
    // ImGui integration
    // Performance metrics
};
```

### Performance Considerations

1. **Mesh Generation**
   - Current: CPU-based mesh generation per chunk
   - Bottleneck: Single-threaded vertex generation
   - Future: GPU compute shader implementation planned
   - Optimization: Face culling reduces vertex count significantly

2. **Memory Usage**
   - Chunk Data: ~16KB per chunk (16x16x16 voxels)
   - Vertex Data: Variable (depends on visible faces)
   - Buffer Management: Dynamic resizing with 1.5x growth factor
   - Future: Compression for inactive regions

3. **Render Pipeline**
   - Command Buffer: Single-time recording per frame
   - Push Constants: Used for frequent updates (MVP, lighting)
   - Depth Testing: Early fragment testing enabled
   - Future: Instancing for repeated structures

### Known Issues & Workarounds

1. **Chunk Loading Artifacts**
   - Issue: Visible seams between chunks
   - Current Workaround: Overlap checking in mesh generation
   - Future Fix: Proper chunk boundary handling

2. **Memory Leaks**
   - Issue: Some resources not properly cleaned up
   - Workaround: Manual cleanup in destructor
   - Future: RAII wrapper implementation

3. **Performance Degradation**
   - Issue: FPS drops with many visible chunks
   - Workaround: Limited view distance
   - Future: LOD system and frustum culling

### Current Limitations

1. **Chunk Management**
   - No chunk serialization/persistence
   - Limited chunk loading range
   - No LOD (Level of Detail) system

2. **Rendering**
   - Basic lighting model
   - No texture support yet
   - Limited voxel types

3. **Performance**
   - Single-threaded mesh generation
   - No frustum culling
   - Basic chunk update system

### Future Work

1. **Core Engine Features**
   - [ ] Implement chunk serialization
   - [ ] Add texture support
   - [ ] Implement LOD system
   - [ ] Add frustum culling
   - [ ] Multi-threaded mesh generation

2. **Graphics Improvements**
   - [ ] Advanced lighting system
   - [ ] Ambient occlusion
   - [ ] Shadow mapping
   - [ ] Post-processing effects

3. **Optimization**
   - [ ] GPU-accelerated mesh generation
   - [ ] Instanced rendering for repeated structures
   - [ ] Chunk caching system
   - [ ] Optimized memory management

4. **Gameplay Features**
   - [ ] Voxel modification system
   - [ ] Physics integration
   - [ ] Procedural generation
   - [ ] World editing tools

## Building from Source

### Prerequisites
- CMake 3.15 or higher
- Vulkan SDK 1.2 or higher
- C++17 compatible compiler
- GLFW3
- GLM

### Build Steps
1. Clone the repository
```bash
git clone https://github.com/yourusername/voxceleron.git
cd voxceleron
```

2. Create build directory
```bash
mkdir build
cd build
```

3. Configure and build
```bash
cmake ..
cmake --build .
```

4. Run the executable
```bash
./Debug/voxceleron.exe  # or Release/voxceleron.exe
```

### Common Build Issues

1. **Vulkan SDK Not Found**
   - Ensure VULKAN_SDK environment variable is set
   - Verify Vulkan SDK installation

2. **Shader Compilation**
   - Shaders are precompiled in the build process
   - Requires glslc compiler from Vulkan SDK
   - Check shader paths in CMakeLists.txt

3. **ImGui Integration**
   - ImGui is included as a submodule
   - Ensure submodules are initialized

## Contributing

Contributions are welcome! Please read our contributing guidelines and code of conduct before submitting pull requests.

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgments

- Vulkan SDK and documentation
- GLFW library
- ImGui library
- GLM library
- Various voxel engine resources and tutorials that inspired this project

