import setup_path
import colosseum

client = colosseum.MultirotorClient()
client.confirmConnection()
client.armDisarm(True)
