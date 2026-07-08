import colosseum

c = colosseum.MultirotorClient()
c.confirmConnection()

c.simSetObjectMaterialFromTexture("OrangeBall", "sample_texture.jpg")

