# Windows PowerShell script that handles the installation of required
# packages and the setup of the development environment. Running this
# script is more or less the equivalent of manually performing the
# steps presented in [1] (Windows).
#
#
# [1]: https://docs.zephyrproject.org/latest/develop/getting_started/index.html

# where to download the boot container from
$BOOT_CONTAINER_URL="https://github.com/NXPHoverGames/frdm-imx93-for-nxpcup/releases/download/rel-v1.0"

# name of the boot container archive
$BOOT_CONTAINER_NAME="frdm-imx93-release-v1.0.zip"

# where to download uuu from
$UUU_URL="https://github.com/nxp-imx/mfgtools/releases/latest/"

# root directory in which packages are installed
$PKG_ROOT="$PWD\packages\"

# list of packages to install
$packages = @(
	[PSCustomObject]@{ name = "Ninja-build.Ninja"; alias = "ninja"; exe_path = ""; }
	[PSCustomObject]@{ name = "Kitware.CMake"; alias = "cmake"; exe_path = "\bin" }
	[PSCustomObject]@{ name = "oss-winget.gperf"; alias = "gperf"; exe_path = "" }
	[PSCustomObject]@{ name = "JernejSimoncic.Wget"; alias = "wget"; exe_path = "" }
	[PSCustomObject]@{ name = "oss-winget.dtc"; alias = "dtc"; exe_path = "\usr\bin" }
	[PSCustomObject]@{ name = "7zip.7zip"; alias = "7z"; exe_path = "" }
	[PSCustomObject]@{ name = "PuTTY.PuTTY"; alias = "putty"; exe_path = "" }
)

# check if python is installed
try {
	$python_version = python --version
	Write-Host "Detected python version: " -NoNewline
	Write-Host "$python_version." -ForegroundColor yellow
	Write-Host "Please make sure your python version is at least 3.10!" -ForegroundColor yellow
} catch {
	throw "No python detected. Please install it (minimum version 3.10) before running this script!"
}

# install required packages
ForEach ($pkg in $packages) {

	try {
		$pkg_path = "$PKG_ROOT$($pkg.alias)"

		# check to see if pacakge is already installed
		$cmd_info = Get-Command $pkg.alias -ErrorAction SilentlyContinue
		if ($cmd_info) {
			Write-Host "Found" -NoNewline
			Write-Host " $($pkg.alias) " -NoNewline -ForegroundColor yellow
			Write-Host "at $($cmd_info.Source)..." -NoNewline
			Write-Host "SKIP" -ForegroundColor yellow
			continue
		}

		Write-Host "Attempting to install $($pkg.name)..." -NoNewline

		# first we install the package in its root folder
		$ret = winget install --id $pkg.name `
				      --silent `
				      --accept-package-agreements `
				      --accept-source-agreements -l $pkg_path
		if ($ret -match "No package found") {
			throw "No package with name `"$($pkg.name)`" found"
		}

		if ($LASTEXITCODE -ne 0) {
			throw "winget returned error code: 0x$($LASTEXITCODE.ToString('X'))"
		}

		# executable might not be in root folder so append the
		# additional bits to form the path to the folder in which
		# the executabe resides
		$pkg_path = $pkg_path + "$($pkg.exe_path)"

		$registry_path = [Environment]::GetEnvironmentVariable("Path", "User") + `
				 [Environment]::GetEnvironmentVariable("Path", "Machine")

		# check if the executable needs to be added to the registry
		if (-not(($registry_path -split ';' | ForEach-Object { $_.TrimEnd("\") }) -contains $pkg_path)) {
			# don't use any other path here to avoid polluting the User PATH
			# make sure there's no trailing ";". We're going to append those ourselves
			$user_path = [Environment]::GetEnvironmentVariable("Path", "User").TrimEnd(';')

			[Environment]::SetEnvironmentVariable("Path", "$user_path;$pkg_path", "User")
		}

		# check if the executable needs to be added to $env:Path
		if (-not(($env:Path -split ';' | ForEach-Object { $_.TrimEnd("\") }) -contains $pkg_path)) {
			$env:Path = $env:Path.TrimEnd(';') + ";" + "$pkg_path"
		}
		
	} catch {
		Write-Host "FAIL" -ForegroundColor red
		throw "Failed to install `"$($pkg.name)`": $_"
	}

	Write-Host "OK" -ForegroundColor green
}

# set up the virtual environment
Write-Host "Attempting to set up virtual env at $PWD\ENV..." -NoNewline

if (-not(Test-Path -path "$PWD\ENV")) {
	try {
		python -m venv ENV
		Write-Host "OK" -ForegroundColor green
	} catch {
		Write-Host "FAIL" -ForegroundColor red
		throw "Failed to set up virtual env: $_"
	}
} else {
	Write-Host "SKIP" -ForegroundColor yellow
}

# activate the environment
Write-Host "Activating virtual environment at $PWD\ENV" -ForegroundColor yellow

. $PWD\ENV\Scripts\Activate.ps1 *> $null

# install west
Write-Host "Installing west..." -NoNewline

if (-not(pip3 list | Select-String "west")) {
	pip3 install west *> $null
	if ($LASTEXITCODE -ne 0) {
		Write-Host "FAIL" -ForegroundColor red
		throw "pip3 returned error code: $LASTEXITCODE"
	}

	Write-Host "OK" -ForegroundColor green
} else {
	Write-Host "SKIP" -ForegroundColor yellow
}

$work_dir = Split-Path $pwd
$crt_dir = $pwd

# initialize west
Write-Host "Initializing west environment at $work_dir..." -NoNewline

if (-not(Test-Path -path "$work_dir\.west")) {
	Push-Location $work_dir

	west init -l "$crt_dir" *> $null
	if ($LASTEXITCODE -ne 0) {
		Write-Host "FAIL" -ForegroundColor red
		Pop-Location
		throw "west init returned error code: $LASTEXITCODE"
	}

	Pop-Location

	Write-Host "OK" -ForegroundColor green
} else {
	Write-Host "SKIP" -ForegroundColor yellow
}

# update the repositories
Write-Host "Updating repositories..." -NoNewline

west update *> $null
if ($LASTEXITCODE -ne 0) {
	Write-Host "FAIL" -ForegroundColor red
	throw "west update returned error code: $LASTEXITCODE"
}

Write-Host "OK" -ForegroundColor green

# export the CMAKE package
west zephyr-export *> $null
if ($LASTEXITCODE -ne 0) {
	throw "west zephyr-export returned error code: $LASTEXITCODE"
}

# install python packages used by west
Write-Host "Installing west packages..." -NoNewline

west packages pip --install *> $null
if ($LASTEXITCODE -ne 0) {
	Write-Host "FAIL" -ForegroundColor red
	throw "west packages returned error code: $LASTEXITCODE"
}

Write-Host "OK" -ForegroundColor green

# install the ARM64 toolchain
Write-Host "Installing the ARM64 toolchain..." -NoNewline
west sdk install --install-base "$work_dir" -t aarch64-zephyr-elf *> $null
if ($LASTEXITCODE -ne 0) {
	Write-Host "FAIL" -ForegroundColor red
	throw "Failed to install toolchain: $LASTEXITCODE"
} else {
	Write-Host "OK" -ForegroundColor green
}

# download and extract the boot image
Write-Host "Downloading and extracting boot container..." -NoNewline
if (-not(Test-Path -path "$crt_dir\packages\$BOOT_CONTAINER_NAME")) {
	curl.exe -f --silent -L --output packages\$BOOT_CONTAINER_NAME "$BOOT_CONTAINER_URL/$BOOT_CONTAINER_NAME"
	if ($LASTEXITCODE -ne 0) {
		Write-Host "FAIL" -ForegroundColor red
		throw "Failed to download boot container: $LASTEXITCODE"
	}

	7z e "packages\$BOOT_CONTAINER_NAME" -o"$crt_dir\boot" "flash.bin" *> $null
	if ($LASTEXITCODE -ne 0) {
		Write-Host "FAIL" -ForegroundColor red
		throw "Failed to extract boot container: $LASTEXITCODE"
	}

	Write-Host "OK" -ForegroundColor green
} else {
	Write-Host "SKIP" -ForegroundColor yellow
}

# check if uuu is present. If not, download it
Write-Host "Downloading uuu..." -NoNewline

if (-not(Test-Path -path "$crt_dir\boot\uuu.exe")) {
	curl.exe -f --silent -L --output boot\uuu.exe "$UUU_URL/download/uuu.exe"
	if ($LASTEXITCODE -ne 0) {
		Write-Host "FAIL" -ForegroundColor red
		throw "Failed to download uuu: $LASTEXITCODE"
	}
	Write-Host "OK" -ForegroundColor green
} else {
	Write-Host "SKIP" -ForegroundColor yellow
}

Write-Host "Using uuu with version: " -NoNewline
.\boot\uuu.exe | ForEach-Object { if ($_ -match 'libuuu_(\S+)') { Write-Host $matches[1] -ForegroundColor yellow } }

Write-Host "Environment ready! Please close the current session so that" `
	   "you can use the correct Path variable" -ForegroundColor green
Write-Host "Please make sure to activate environment by" `
	   "running `"$PWD\ENV\Scripts\Activate.ps1`"" -ForegroundColor yellow
