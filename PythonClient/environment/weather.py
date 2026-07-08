import setup_path 
import colosseum


client = colosseum.VehicleClient()
client.confirmConnection()

client.simEnableWeather(True)

colosseum.wait_key('Press any key to enable rain at 25%')
client.simSetWeatherParameter(colosseum.WeatherParameter.Rain, 0.25);

colosseum.wait_key('Press any key to enable rain at 75%')
client.simSetWeatherParameter(colosseum.WeatherParameter.Rain, 0.75);

colosseum.wait_key('Press any key to enable snow at 50%')
client.simSetWeatherParameter(colosseum.WeatherParameter.Snow, 0.50);

colosseum.wait_key('Press any key to enable maple leaves at 50%')
client.simSetWeatherParameter(colosseum.WeatherParameter.MapleLeaf, 0.50);

colosseum.wait_key('Press any key to set all effects to 0%')
client.simSetWeatherParameter(colosseum.WeatherParameter.Rain, 0.0);
client.simSetWeatherParameter(colosseum.WeatherParameter.Snow, 0.0);
client.simSetWeatherParameter(colosseum.WeatherParameter.MapleLeaf, 0.0);

colosseum.wait_key('Press any key to enable dust at 50%')
client.simSetWeatherParameter(colosseum.WeatherParameter.Dust, 0.50);

colosseum.wait_key('Press any key to enable fog at 50%')
client.simSetWeatherParameter(colosseum.WeatherParameter.Fog, 0.50);

colosseum.wait_key('Press any key to disable all weather effects')
client.simEnableWeather(False)
