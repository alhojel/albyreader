# Genie 1, 2, 3 & Waymo World Model

Technical overview of the Genie lineage (Google DeepMind) and its application to autonomous driving via Waymo.

---

## Genie 1: Generative Interactive Environments (Feb 2024)

**Paper:** [arxiv.org/abs/2402.15391](https://arxiv.org/abs/2402.15391)
**Authors:** Jake Bruce, Michael Dennis, Ashley Edwards, Jack Parker-Holder, et al. (Google DeepMind)

The first generative interactive environment trained unsupervised from unlabelled Internet videos. 11B parameters total. Can be prompted with text, synthetic images, photographs, or sketches to generate action-controllable virtual worlds.

### Architecture

Three components:

**Spatiotemporal Video Tokenizer (ST-ViViT)**
- VQ-VAE with spatiotemporal transformer encoder/decoder
- Encoder: 12 layers, 512 d_model, 8 heads
- Decoder: 20 layers, 1024 d_model, 16 heads
- Codebook: 1,024 codes, patch size 4, latent dimension 32
- Temporal-aware tokenization (not spatial-only) — key design choice that beat alternatives
- 205M parameters, 0.9GB memory

**Latent Action Model (LAM)**
- 300M parameters, ST-transformer encoder-decoder
- Codebook size: 8 discrete actions (VQ-VAE based)
- Encoder processes all frames plus next frame; decoder reconstructs next frame from history and latent actions, forcing actions to encode meaningful changes
- Uses raw pixel inputs (not tokenized) for better controllability
- No ground-truth action labels needed — learns actions unsupervised

**Dynamics Model**
- 10.1B parameter MaskGIT decoder-only transformer using ST-transformer
- Interleaved spatial and temporal attention layers — spatial attends over 1xHxW tokens within each timestep, temporal attends over Tx1x1 tokens across T timesteps
- Reduces complexity from quadratic to linear with respect to frame count
- Cross-entropy loss with random masking (50-100% Bernoulli)
- Inference: 25 MaskGIT sampling steps per frame at temperature 2

### Training

- **Dataset:** 6.8M curated videos (30,000 hours) of 2D platformer gameplay from public Internet video
- **Curation:** Hand-labeled 10k videos (quality 1-5), trained 11M ResNet18 classifier, filtered 55M original videos down to 6.8M — curated subset outperformed full dataset
- **Video specs:** 16-second clips at 10 FPS, 160x90 resolution
- **Tokenizer training:** 300k steps, AdamW with cosine decay, lr 3e-4, batch 384 on 64 TPUv3
- **Dynamics + LAM co-training:** 125k steps, batch 512, on 256 TPUv5p, 942B tokens total
- **Stabilization:** bfloat16, QK normalization

### Results

- Scaling from 40M to 2.7B parameters showed consistent improvement
- 11B Platformers model achieved ~40 FVD (Fréchet Video Distance)
- Controllability measured via ΔtPSNR (true actions vs random actions) — higher = better control
- Robotics variant: 2.5B params, ~130k robot demonstrations, learned consistent semantic actions without labels
- Behavioral cloning: LAM-based policy matched oracle performance with as few as 200 expert samples on CoinRun

### Tokenizer Ablation

| Architecture | Params | Memory | FVD | ΔtPSNR |
|-------------|--------|--------|-----|--------|
| ViT (spatial-only) | 230M | 0.3GB | 114.5 | 1.39 |
| C-ViViT | 225M | 1.6GB | 272.7 | 1.37 |
| ST-ViViT (Genie) | 205M | 0.9GB | 81.4 | 1.66 |

### LAM Input Ablation

| Input | FVD (Platformers) | ΔtPSNR (Platformers) | FVD (Robotics) | ΔtPSNR (Robotics) |
|-------|-------------------|----------------------|----------------|-------------------|
| Token-input | 38.8 | 1.33 | 257.8 | 1.65 |
| Pixel-input | 40.1 | 1.91 | 136.4 | 2.07 |

Pixel-input superior for controllability despite slightly higher FVD on Platformers.

### Emergent Capabilities

- Parallax effect emulation
- Deformable object physics
- Generalization to out-of-distribution prompts (text-generated images, sketches, photographs)
- Consistent latent action semantics across diverse initial frames

---

## Genie 2: Large-Scale Foundation World Model (Dec 2024)

**Source:** [deepmind.google/discover/blog/genie-2](https://deepmind.google/discover/blog/genie-2-a-large-scale-foundation-world-model/)
**Authors:** Jack Parker-Holder, Philip Ball, Jake Bruce, et al. (Google DeepMind)

Generates diverse, action-controllable 3D environments from single image prompts. Major leap from 2D platformers to full 3D worlds.

### Architecture

- **Type:** Autoregressive latent diffusion model trained on large-scale video
- **Components:** Autoencoder for frame compression into latent space + large transformer-based dynamics model with causal masking (LLM-style architecture)
- **Controllability:** Classifier-free guidance for improved action response
- **Inference:** Frame-by-frame autoregressive sampling consuming actions and prior latent frames

### Key Capabilities

**Action Responsiveness:** Intelligently interprets keyboard and mouse inputs, identifying which entities should respond to actions (characters vs. environment).

**Counterfactual Generation:** Multiple divergent trajectories from identical starting frames with different actions.

**Extended Consistency:** Spatial memory for environments lasting up to one minute — accurately renders previously unseen areas when revisited.

**Emergent Physics:**
- Water and smoke effects
- Gravity simulation
- Object interactions (doors, balloons)
- Character animation and NPC behavior
- Lighting, reflections, bloom

**Perspective Support:** First-person, isometric, and third-person camera angles.

**Real-world Prompts:** Handles both synthetic and photographic image prompts.

### Agent Integration

SIMA agent successfully completed tasks in Genie 2-generated environments following natural language instructions.

---

## Genie 3: Real-Time Interactive General-Purpose World Model (Aug 2025)

**Source:** [deepmind.google/blog/genie-3](https://deepmind.google/blog/genie-3-a-new-frontier-for-world-models/)

First real-time interactive general-purpose world model. Not specific to any particular environment.

### Architecture

- **Model:** 11B-parameter auto-regressive transformer
- **Resolution:** 720p
- **Frame rate:** 24 FPS real-time
- **Consistency:** Several minutes continuous interaction
- **Visual memory:** Extends ~1 minute back
- **Inputs:** Text prompts, images, photos, sketches

### Technical Approach

- Builds worlds one frame at a time, each frame conditioned on all previous frames
- No hard-coded physics engine — model learns physics from data (object movement, falling, interactions)
- Physical consistency maintained through memory of previously generated content
- Auto-regressive generation (harder than video generation since inaccuracies accumulate over time)
- Processes user inputs multiple times per second

### Key Difference from Predecessors

Unlike NeRF/Gaussian Splatting requiring explicit 3D representations, Genie 3 generates frame-by-frame from world description and actions. Unlike Sora which produces fixed-length non-interactive clips, Genie 3 worlds are interactive and open-ended.

### Limitations

- Few minutes maximum continuous interaction
- Expensive to run (~$1,000/month reported for access)
- Prompt adherence and physics not always perfect
- Character controllability and latency issues
- Text rendering only when provided in input
- Struggles with multiple independent agents

### Agent Training

SIMA agent tested in Genie 3 environments — pursued goals like "approach the bright green trash compactor" in warehouse settings and achieved the goal in all cases.

DeepMind position: "We think world models are key on the path to AGI, specifically for embodied agents, where simulating real world scenarios is particularly challenging."

---

## Waymo World Model: Genie 3 Fine-tuned for Autonomous Driving (Feb 2026)

**Source:** [waymo.com/blog/2026/02/the-waymo-world-model](https://waymo.com/blog/2026/02/the-waymo-world-model-a-new-frontier-for-autonomous-driving-simulation)

Built on Genie 3, adapted to produce photorealistic, controllable, multi-sensor driving scenes at scale.

### Key Innovation: 2D-to-3D Transfer

Rather than training from scratch on fleet data alone, leverages Genie 3's world knowledge from diverse video pretraining. Specialized post-training forces the model to generate **both 2D video and 3D lidar outputs simultaneously** for the same scene — transferring 2D video understanding into 3D sensor space unique to Waymo's hardware suite.

Generates 4D point clouds providing precise depth signals.

### Three Control Mechanisms

1. **Driving Action Control:** "What if" counterfactual simulations — test alternative driving decisions beyond recorded trajectories while maintaining realism
2. **Scene Layout Control:** Selective placement of road users, road mutations, traffic signal states
3. **Language Control:** Time-of-day, weather conditions, or entirely synthetic scenes from text

### Data Augmentation

Can convert ordinary dashcam or mobile phone video into multimodal simulations showing how the Waymo Driver would perceive a scene with its full sensor suite.

### Scale

- ~720p, 20-24 FPS real-time
- Waymo reports ~200M fully autonomous miles on public roads; Driver trains/evaluates on billions of additional virtual miles
- Waymo World Model is now the main engine generating those virtual worlds
- Explicit goal: expose the stack to rare, safety-critical "long-tail" events (tornadoes, floods, snow, unusual obstacles)
- Efficient variant available with dramatic compute reduction for longer simulations

### Competitive Landscape

- **Wayve:** GAIA-1/GAIA-2 generative world models for autonomy
- **Tesla:** Dojo supercomputer + massive real-world video, less emphasis on fully generative simulators
- **NVIDIA:** Cosmos world foundation models (partners: Plus, Oxa)
- **Pony.ai:** PonyWorld generative capabilities with Moore Threads

---

## References

1. [Genie 1 paper (Bruce et al., 2024)](https://arxiv.org/abs/2402.15391)
2. [Genie 1 HTML paper](https://arxiv.org/html/2402.15391v1)
3. [Genie 2 blog (DeepMind, Dec 2024)](https://deepmind.google/discover/blog/genie-2-a-large-scale-foundation-world-model/)
4. [Genie 3 blog (DeepMind, Aug 2025)](https://deepmind.google/blog/genie-3-a-new-frontier-for-world-models/)
5. [Genie 3 model page](https://deepmind.google/models/genie/)
6. [Genie 3 technical site](https://www.genie3.cloud/)
7. [Project Genie consumer product](https://blog.google/innovation-and-ai/models-and-research/google-deepmind/project-genie/)
8. [Waymo World Model blog (Feb 2026)](https://waymo.com/blog/2026/02/the-waymo-world-model-a-new-frontier-for-autonomous-driving-simulation)
9. [GAIA-1 (Wayve, Hu et al., 2023)](https://arxiv.org/abs/2309.17080)
10. [DriVerse navigation world model](https://arxiv.org/html/2504.18576v1)
