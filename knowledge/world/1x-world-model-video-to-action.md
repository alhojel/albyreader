# 1X World Model: From Video to Action

**Source:** [1x.tech/discover/world-model-self-learning](https://www.1x.tech/discover/world-model-self-learning)

---

## Overview

Home robots must demonstrate common sense behavior and develop a thorough understanding of physical environments. Traditional robot foundation models — vision-language-action models (VLAs) — combine pretrained vision-language models with action prediction heads. However, this approach emphasizes visual and semantic understanding rather than physical dynamics prediction, requiring tens of thousands of hours of expensive robot data to accomplish tasks humans perform effortlessly.

1X introduces **1XWM**, a video-pretrained world model integrated into NEO's policy framework. Rather than predicting actions directly from static images and language, this system derives robot actions from text-conditioned video generation. By harnessing world dynamics from internet-scale video, the model generalizes to novel objects, motions, and tasks without requiring large-scale robot pretraining or teleoperated demonstrations.

---

## The Humanoid Advantage

Internet video encodes structural reality priors — how people and objects move, force application points, and interaction constraints. Converting video into actionable control depends on embodiment that matches human kinematic and dynamic properties.

NEO's humanoid form ensures hardware serves as a foundational AI component, closing the human-robot translation gap. Human-like compliance produces interaction dynamics (friction, inertia, contact behavior) matching human motion closely enough for learned model priors to remain distributionally valid.

---

## From Video Knowledge to World Models

Frontier text-to-video models like Veo and Sora generate highly realistic videos, yet zero-shot generations lack robot embodiment grounding. Critical requirements for control:

**Visual/Spatial**: Consistency with robot camera intrinsics, egocentric viewpoint, depth preservation, and precise spatial manipulation relationships

**Kinematic**: Robot movements feasible within embodiment constraints — morphology, joint limits, velocities, actuation capabilities

**Physics**: Avoidance of physically impossible outcomes (object teleportation) preventing real-world success

### Two-Stage Grounding

**World Model Backbone (WM)**: Text-conditioned diffusion model trained on web-scale video, mid-trained on egocentric human data, fine-tuned on NEO sensorimotor logs. Predicts high-fidelity scene evolution with strong visual, spatial, and physical accuracy.

**Inverse Dynamics Model (IDM)**: Bridges pixels to actuators by predicting exact action sequences transitioning between generated frames. Metrics and rejection sampling enforce kinematic correctness.

At inference: text prompt + starting frame -> WM generates intended future frames -> IDM extracts trajectories -> NEO executes.

---

## Training and Inference Recipe

The 1XWM backbone uses a **14-billion parameter** generative video model adapted to NEO through multi-stage training:

**Egocentric Mid-training**: 900 hours of first-person manipulation video aligns the model with human manipulation tasks, capturing general behaviors though struggling with NEO-specific generations.

**Embodiment Fine-tuning**: 70 hours of robot data adapts the model to NEO's visual appearance and kinematics.

**Caption quality** significantly impacts prompt adherence. VLM-driven caption upsampling generates detailed descriptions from brief task descriptions, matching the detailed conditioning used during video model pretraining.

**Dynamic time warping** evaluation metric compares ground truth future action sequences against IDM-extracted actions from generated video, filtering for generations that look visually convincing while yielding accurate actions.

### IDM Details

- Two-image prediction with W=8 sliding window
- Depth Anything backbone with separate flow matching head (replaces single diffusion transformer)
- Trained on 400 hours of unfiltered robot data including random play and non-task motions
- At test-time: IDM outputs undergo timewise averaging across noise batches and sliding windows for smooth trajectories

### Post-training Dataset

- 98.5% pick-and-place data (69.4% pick, 29.3% place, 1.3% other)
- Filtered for tabletop manipulation with visible hands
- Web-scale pretraining enables generalization to unseen objects, environments, and tasks

---

## Capabilities and Results

1XWM generates videos strongly aligning with real-world execution, demonstrating spatial, kinematic, and physical understanding. Tested on:
- In-distribution and out-of-distribution object grasping
- Manipulating unseen objects with interesting affordances
- Entirely novel tasks requiring new motions

### Limitations

- Occasional excessive task completion optimism
- Weak depth understanding from monocular pretraining
- Subtle constraint violations: object inconsistency, depth errors, geometry problems, contact issues
- Post-training substantially reduces errors, but depth or stereo integration needed

### Video Quality and Task Success

Visual generation quality correlates with task success probability. Test-time compute approaches improve success: generating multiple rollouts in parallel and executing the highest quality generation boosted "pull tissue" task success from **30% (single generation) to 45% (eight generations)**.

---

## Egocentric Data and Caption Ablations

Three evaluation datasets of 500 image-prompt pairs:
- **In-distribution**: Hard tasks and backgrounds matching robot training data
- **New Tasks**: Novel tasks (whisking bowls, pulling tissue, bimanual manipulation)
- **OOD T2I**: Pick tasks with out-of-distribution household objects and backgrounds

**Caption upsampling** improved generation quality across all evaluation splits.

**Egocentric human data** improved generation quality on New Tasks and OOD splits — consistent with hypothesis that ego human data provides transferable manipulation priors mapping well to NEO's humanoid embodiment. For in-distribution tasks, egocentric data may dilute post-training mix.

---

## Toward Full Autonomy

Current 1XWM backbone requires 11 seconds inference (multi-GPU), generating 5 seconds real-time video with 1-second IDM action extraction. Future improvements:
- Faster inference for reactive task solving and contact-rich manipulation
- Tasks exceeding 5 seconds require closed-loop replanning with memory context
- Nonzero success across broad task sets provides self-improvement leverage

1XWM establishes a flywheel where exploration, evaluation, and policy refinement derive from NEO's own experience rather than expert demonstration limitation.

---

## References

1. [PI0.6 robot foundation model](https://www.pi.website/blog/pistar06)
2. [Helix robot learning system (Figure AI)](https://www.figure.ai/news/helix)
3. [Groot N1.5 (NVIDIA)](https://research.nvidia.com/labs/gear/gr00t-n1_5/)
4. [MolmoAct auxiliary objectives](https://arxiv.org/pdf/2508.07917)
5. [Gemini-Robotics](https://deepmind.google/models/gemini-robotics/gemini-robotics/)
6. [Veo (Google)](https://aistudio.google.com/models/veo-3)
7. [Sora (OpenAI)](https://openai.com/index/sora-2/)
8. [DreamGen video grounding (NVIDIA)](https://research.nvidia.com/labs/gear/dreamgen/)
9. [UniPi inverse dynamics](https://arxiv.org/abs/2302.00111)
10. [DALL-E 3 caption adherence](https://cdn.openai.com/papers/dall-e-3.pdf)
11. [Dynamic time warping](https://en.wikipedia.org/wiki/Dynamic_time_warping)
12. [Depth Anything v2](https://depth-anything-v2.github.io/)
13. [Test-time compute efficiency](https://arxiv.org/pdf/2408.03314)
