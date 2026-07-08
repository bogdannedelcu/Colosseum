# Script parameters
$colosseumExecutable = "c:\Colosseum\Blocks\blocks.exe"
$colosseumProcessName = "Blocks"

# Ensure proper path
$env:Path = 
    [System.Environment]::GetEnvironmentVariable("Path","Machine") + ";" +
    [System.Environment]::GetEnvironmentVariable("Path","User")

# Install python app requirements
pip3 install -r .\app\requirements.txt

# Overwrite Colosseum configuration
New-Item -ItemType Directory -Force -Path $env:USERPROFILE\Documents\Colosseum\
copy .\app\settings.json $env:USERPROFILE\Documents\Colosseum\

# Kill previous Colosseum instance
Stop-Process -Name $colosseumProcessName -Force -ErrorAction SilentlyContinue
sleep 2

# Start new Colosseum instance
Start-Process -NoNewWindow  -FilePath $colosseumExecutable -ArgumentList "-RenderOffScreen"
echo "Starting the Colosseum environment has completed."
