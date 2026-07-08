$colosseumInstallPath = "C:\Colosseum\"
$colosseumBinaryZipUrl =  "https://github.com/CodexLabsLLC/Colosseum/releases/download/v1.3.1-windows/Blocks.zip"
$colosseumBinaryZipFilename = "Blocks.zip"
$colosseumBinaryPath = $colosseumInstallPath + "blocks\blocks\binaries\win64\blocks.exe"
$colosseumBinaryName = "Blocks"

$webClient = new-object System.Net.WebClient

# Install the OpenSSH Client
Add-WindowsCapability -Online -Name OpenSSH.Client~~~~0.0.1.0
# Install the OpenSSH Server
Add-WindowsCapability -Online -Name OpenSSH.Server~~~~0.0.1.0
# Enable service
Start-Service sshd
Set-Service -Name sshd -StartupType 'Automatic'

#Install Chocolatey
Set-ExecutionPolicy Bypass -Scope Process -Force; [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; iwr https://chocolatey.org/install.ps1 -UseBasicParsing | iex
# Bypass confirmation in scripts.
choco feature enable --name="'allowGlobalConfirmation'"
choco install python --version=3.8.2
choco install git
# Run time c++
choco install vcredist-all
choco install directx

#Create new folder & set as default directory
New-Item -ItemType directory -Path $colosseumInstallPath
cd $colosseumInstallPath

# Get Colosseum
$webClient.DownloadFile($colosseumBinaryZipUrl, $colosseumInstallPath + $colosseumBinaryZipFilename)
# Unzip Colosseum
Expand-Archive $colosseumBinaryZipFilename

# Firewall rule for Colosseum
New-NetFirewallRule -DisplayName $colosseumBinaryName -Direction Inbound -Program $colosseumBinaryPath -Action Allow
