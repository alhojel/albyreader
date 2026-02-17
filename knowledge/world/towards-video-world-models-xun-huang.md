# Towards Video World Models

**Author:** Xun Huang | **Date:** July 11, 2025
**Source:** [xunhuang.me/blogs/world_model.html](https://www.xunhuang.me/blogs/world_model.html)

---

## What Are World Models?

### Understanding versus Simulating the World

A world model predicts environmental evolution given current state and action. However, no consensus exists on what constitutes "state." Two paradigms exist:

1. **Internal/abstract models** — mirror human cognitive prediction at semantic levels. LLMs show promise but textual-only knowledge leaves gaps. True grounded models need vision, action, and interaction. Multimodal models like LWM and Meta's V-JEPA 2 advance this by predicting abstract video representations.

2. **High-fidelity simulation** — detailed, complete world description, essentially a Matrix-like simulator. Enables robots learning without costly real-world interaction, and immersive human experiences. Quality visuals matter: robots need detailed inputs for sim-to-real transfer; humans need realism for immersion.

**Core insight:** Internal understanding models and external simulation models can work synergistically in embodied AI contexts.

Approaches for high-fidelity simulation include **video world models**, physics-based platforms, and neural 3D/4D techniques. This post focuses on video world models.

---

## Video World Models

Recent progress shows state-of-the-art systems generating photorealistic details with physically realistic dynamics (Google's Veo3). Google DeepMind CEO Demis Hassabis hinted at interactive world models for gaming via video generation.

Yet current video generators aren't true world models. **A true video world model must be: causal, interactive, persistent, real-time, and physically accurate.**

### Causality

Causality represents time's unidirectional nature — past determines future, not vice versa.

State-of-the-art video generation uses diffusion transformers (DiTs) with non-causal, bidirectional attention generating fixed frame sets simultaneously. This violates temporal asymmetry: future information influences past, eliminating dynamic interaction possibilities post-initial frame.

Autoregressive, frame-by-frame generation is the **first step toward true world models**, enabling real-time interaction. LLM-style autoregressive models (CogVideo, VideoPoet) are inherently causal but quality-limited due to lossy vector quantization. Single-token generation is slow and poorly parallelizable.

**Solution:** Combine diffusion quality/speed with autoregressive causality.

- [Diffusion Forcing](https://arxiv.org/abs/2407.01392) — per-frame independent noise enables autoregressive capabilities
- [CausVid](https://causvid.github.io/) — bidirectional-attention DiTs can adapt to autoregressive diffusion transformers (AR-DiT) with causal attention
- [MAGI-1](https://github.com/SandAI-org/MAGI-1) — from-scratch pretraining with infrastructure improvements

### Interactivity

Interactive controllability defines video world models — **the ability to influence future through on-the-fly actions**. Actions vary: gamers manipulate objects, robots execute motor commands, autonomous vehicles steer. Unified requirement: models respond dynamically to external interventions.

Early approaches used recurrent variational autoencoders (Ha & Schmidhuber 2018; Hafner et al. 2020, 2021). Recent work shifted toward action-conditioned autoregressive diffusion models, trained on increasingly realistic environments.

Central challenge: acquiring training data aligning video frames with actions. Gaming provides annotated data; leveraging unlabeled internet-scale video remains open. [Genie](https://sites.google.com/view/genie-2024/home) explored unsupervised action learning; [Genie-2](https://deepmind.google/discover/blog/genie-2-a-large-scale-foundation-world-model/) leveraged AR-DiT architecture with impressive results.

### Persistence

World models should produce **indefinitely long videos** while maintaining **consistency across time**. Scaling alone seems insufficient — transformers with extremely long context, trained on long-duration data, theoretically maintain temporal coherence, but real-time responsiveness complicates things. Unlike LLMs tolerating increased inference time, video world models face hard latency constraints.

Proposed techniques:
- Aggressive temporal compression of distant frames ([FAR](https://farlongctx.github.io/), [FramePack](https://lllyasviel.github.io/frame_pack_gitpage/)) — works when recent frames suffice but fails for "needle in haystack" scenarios
- Video generation with persistent 3D conditions — effective for static scenes, struggles with dynamics
- Linear RNNs/state-space models capturing long-range dependencies without increasing per-frame generation time

### Real-Time

Two metrics matter: **throughput** (frames/second generated) and **latency** (delay between action and responsive frame).

Acceptable thresholds vary: ~1 second for live streaming, 0.1s for gaming, 0.01s for VR.

**Critical insight:** Non-causal diffusion models generating t-second videos simultaneously have minimum t-second latency. Non-causal approaches cannot achieve truly real-time latency regardless of throughput.

Solution: Integrate causal, frame-by-frame autoregressive modeling with fast, few-step distilled diffusion.

- [CausVid](https://causvid.github.io/) — pioneered using Distribution Matching Distillation (DMD)
- [Self-Forcing](https://self-forcing.github.io/) — simulates inference (autoregressive rollout with KV caching) during training, achieving real-time throughput (17FPS) and subsecond latency on single GPU
- [APT2](https://seaweed-apt.com/2) — similar training-time rollout; demonstrates real-time models finetune with interactive controls
- Startups: [Decart](https://oasis.decart.ai/welcome), [Odyssey](https://experience.odyssey.world/), [Wayfarer Labs](https://wayfarerlabs.ai/blog), [Dynamics Lab](https://blog.dynamicslab.ai/)

### Physical Accuracy

Ultimate goal: simulators indistinguishable from reality — generating visually pleasing pixels adhering to real-world physics.

Physical realism improves with scaling, but generalizability of learned physics laws remains unclear. [Kang et al.](https://phyworld.github.io/) tested with 2D physics-simulated datasets: combinatorial generalization achieves; extrapolative scenarios fail when observed properties exceed training distribution.

Enhancement techniques:
- [VideoJAM](https://hila-chefer.github.io/videojam-paper.github.io/) — pixel reconstruction alone insufficient for physical coherence; adds optical flow denoising objective
- Dataset curation for rare events: [Ctrl-Crash](https://anthonygosselin.github.io/Ctrl-Crash-ProjectPage/) (car accidents), [PISA](https://github.com/vision-x-nyu/pisa-experiments) (object drops)

### Summary of Properties

Five key properties, roughly from fundamental to challenging:

1. **Causality** (hard constraint)
2. **Interactivity** (hard constraint — depends on causality)
3. **Persistence** (soft constraint — spectrum)
4. **Real-time Responsiveness** (trade-off with physical accuracy)
5. **Physical Accuracy** (trade-off with real-time)

Larger models: more physically accurate, slower. Smaller models: real-time capable, less accurate.

- For human entertainment: real-time essential, physical accuracy "good enough to fool eyes"
- For robotic learning: physical accuracy paramount, real-time training unnecessary

---

## Other Approaches to World Simulation

- **Physics-based simulators** ([MuJoCo](https://github.com/google-deepmind/mujoco), [Isaac](https://developer.nvidia.com/isaac), [Genesis](https://github.com/Genesis-Embodied-AI/Genesis)): Consistent, physically realistic; require manual design; struggle with photorealism/real-time rendering
- **Neural 3D methods** ([Wonderland](https://snap-research.github.io/wonderland/), [WonderWorld](https://kovenyu.com/wonderworld/), [Bolt3D](https://szymanowiczs.github.io/bolt3d), [World Labs](https://www.worldlabs.ai/blog), [SpAItial](https://www.spaitial.ai/)): 3D-consistent, fast rendering; struggle with dynamic scenes
- **Video models**: Highly adaptable, photorealistic; 3D consistency + real-time rendering remains open

Short-term: hybrid models combining paradigm strengths. [WonderPlay](https://kyleleey.github.io/WonderPlay/) combines physics simulators, 3D Gaussian representations, and video diffusion.

---

## Conclusion

"World model" is overloaded terminology describing abstract high-level prediction or detailed reality-indistinguishable simulation. Interactive media powered by world models and new robotic learning paradigms represent next frontiers.

---

## References

1. [Diffusion for World Modeling (Alonso et al., 2024)](https://arxiv.org/abs/2405.12399)
2. [V-JEPA 2 (Assran et al., 2025)](https://ai.meta.com/blog/v-jepa-2-world-model-benchmarks/)
3. [Navigation World Models (Bar et al., 2025)](https://arxiv.org/abs/2412.03572)
4. [Genie: Generative Interactive Environments (Bruce et al., 2024)](https://arxiv.org/abs/2402.15391)
5. [GameGen-X (Che et al., 2025)](https://gamegen-x.github.io/)
6. [VideoJAM (Chefer et al., 2025)](https://arxiv.org/abs/2502.02492)
7. [Diffusion Forcing (Chen et al., 2024)](https://arxiv.org/abs/2407.01392)
8. [The Matrix: Infinite-Horizon World Generation (Feng et al., 2024)](https://arxiv.org/abs/2412.03568)
9. [Ctrl-Crash (Gosselin et al., 2025)](https://arxiv.org/abs/2506.00227)
10. [Long-Context Autoregressive Video Modeling (Gu et al., 2025)](https://arxiv.org/abs/2503.19325)
11. [Recurrent World Models Facilitate Policy Evolution (Ha & Schmidhuber, 2018)](https://papers.nips.cc/paper/7512-recurrent-world-models-facilitate-policy-evolution)
12. [Dream to Control (Hafner et al., 2020)](https://arxiv.org/abs/1912.01603)
13. [Mastering Atari with Discrete World Models (Hafner et al., 2021)](https://arxiv.org/abs/2010.02193)
14. [CogVideo (Hong et al., 2022)](https://arxiv.org/abs/2205.15868)
15. [GAIA-1 (Hu et al., 2023)](https://arxiv.org/abs/2309.17080)
16. [Self-Forcing (Huang et al., 2025)](https://arxiv.org/abs/2506.08009)
17. [How Far Is Video Generation from World Model? (Kang et al., 2024)](https://arxiv.org/abs/2411.02385)
18. [VideoPoet (Kondratyuk et al., 2023)](https://sites.research.google/videopoet/)
19. [Hunyuan-GameCraft (Li et al., 2025)](https://arxiv.org/abs/2506.17201)
20. [WonderPlay (Li et al., 2025)](https://arxiv.org/abs/2505.18151)
21. [PISA: Physics Post-Training (Li et al., 2025)](https://arxiv.org/pdf/2503.09595)
22. [Wonderland (Liang et al., 2025)](https://arxiv.org/abs/2412.12091)
23. [Autoregressive Adversarial Post-Training (Lin et al., 2025)](https://arxiv.org/abs/2506.09350)
24. [LWM: World Model on Million-Length Video (Liu et al., 2024)](https://arxiv.org/abs/2402.08268)
25. [Genie-2 (Parker-Holder et al., 2025)](https://deepmind.google/discover/blog/genie-2-a-large-scale-foundation-world-model/)
26. [Long-Context State-Space Video World Models (Po et al., 2025)](https://arxiv.org/abs/2505.20171)
27. [GEN3C (Ren et al., 2025)](https://arxiv.org/abs/2503.03751)
28. [Bolt3D (Szymanowicz et al., 2025)](https://szymanowiczs.github.io/bolt3d)
29. [MAGI-1 (Teng et al., 2025)](https://arxiv.org/abs/2505.13211)
30. [Diffusion Models Are Real-Time Game Engines (Valevski et al., 2024)](https://arxiv.org/abs/2408.14837)
31. [Video World Models with Long-term Spatial Memory (Wu et al., 2025)](https://arxiv.org/abs/2506.05284)
32. [Distribution Matching Distillation (Yin et al., 2024)](https://arxiv.org/abs/2311.18828)
33. [CausVid (Yin et al., 2025)](https://arxiv.org/abs/2412.07772)
34. [WonderWorld (Yu et al., 2024)](https://arxiv.org/abs/2406.09394)
35. [World-Consistent Video Diffusion with 3D Modeling (Zhang et al., 2024)](https://arxiv.org/abs/2412.01821)
36. [FramePack (Zhang et al., 2025)](https://arxiv.org/abs/2504.12626)
37. [Learning 3D Persistent Embodied World Models (Zhou et al., 2025)](https://arxiv.org/abs/2505.05495)
