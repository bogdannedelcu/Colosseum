from gym.envs.registration import register

register(
    id="colosseum-drone-sample-v0", entry_point="airgym.envs:ColosseumDroneEnv",
)

register(
    id="colosseum-car-sample-v0", entry_point="airgym.envs:ColosseumCarEnv",
)
