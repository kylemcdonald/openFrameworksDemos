# openFrameworksDemos

This repository is a collection of assorted demos and examples using [openFrameworks](http://openframeworks.cc/). There is no theme, except that they each demonstrate a technique that doesn't really fit in any other repository.

## Descriptions

* BarnesHut: basic 2d implementation of the [Barnes-Hut](https://en.wikipedia.org/wiki/Barnes%E2%80%93Hut_simulation) quadtree particle system optimization, good for simulating large force-at-a-distance systems.
* BinnedParticleSystem: basic 2d binning optimization for particle-particle interaction.
* BulgeEffect: magnification effect with GLSL for generating and applying the displacement map.
* BulgeEffectMulti: magnification effect for many points by rendering displacement images into a big displacement FBO before applying them.
* GLSLParticleSystem: example of using shaders to compute one-to-many interaction.
* LutExample: applying a 3-channel lookup table to a live stream using a fragment shader.
* NoiseFlocking: "[Clouds are Looming](http://www.openprocessing.org/sketch/6753)" style noise flocking simulation.
* P52OF: port of a simple noise flocking simulation from Processing, done by pasting the code directly and creating a `PApp` class that wraps OF to make it look more like Processing.
* RemapExample: per-pixel displacement example using a fragment shader.
* VideoShuffler: loads a collection of videos from the `videos/` folder as specified by the `videos.xml` file.