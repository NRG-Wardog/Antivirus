$ErrorActionPreference = 'Stop'

$repo = Split-Path -Parent $PSScriptRoot
Set-Location $repo

$work = Join-Path $env:TEMP ("antivirus-smoke-" + [guid]::NewGuid().ToString('N'))
New-Item -ItemType Directory -Path $work | Out-Null
New-Item -ItemType Directory -Path (Join-Path $work 'scan\nested') | Out-Null

try {
    $signature = Join-Path $work 'signature.bin'
    [System.IO.File]::WriteAllBytes($signature, [System.Text.Encoding]::ASCII.GetBytes('MALWARE'))

    [System.IO.File]::WriteAllText((Join-Path $work 'scan\clean.txt'), 'this file is clean')
    [System.IO.File]::WriteAllText((Join-Path $work 'scan\nested\infected.bin'), 'MALWARE payload fixture')
    [System.IO.File]::WriteAllText((Join-Path $work 'scan\nested\late.bin'), ('x' * 128) + 'MALWARE')
    [System.IO.File]::WriteAllBytes((Join-Path $work 'scan\empty.bin'), [byte[]]@())
    [System.IO.File]::WriteAllText((Join-Path $work 'scan\short.bin'), 'MAL')

    $source = Join-Path $repo 'antivirus_files\prog.c'
    $exe = Join-Path $work 'antivirus.exe'

    cl /nologo /W4 /TC $source /Fe:$exe
    if ($LASTEXITCODE -ne 0) { throw 'MSVC build failed' }

    Push-Location $work
    try {
        '0' | & $exe (Join-Path $work 'scan') $signature | Out-Null
        if ($LASTEXITCODE -ne 0) { throw 'Normal scan failed' }

        $normalLog = Get-Content (Join-Path $work 'AntiVirusLog.txt') -Raw
        if ($normalLog -notmatch 'infected\.bin Infected!') { throw 'Nested infected file was not detected' }
        if ($normalLog -notmatch 'late\.bin Infected!') { throw 'Full scan missed a late signature' }
        if ($normalLog -notmatch 'clean\.txt clean') { throw 'Clean file was not reported clean' }
        if ($normalLog -notmatch 'empty\.bin clean') { throw 'Empty file was not handled as clean' }
        if ($normalLog -notmatch 'short\.bin clean') { throw 'File shorter than the signature was not handled as clean' }

        '1' | & $exe (Join-Path $work 'scan') $signature | Out-Null
        if ($LASTEXITCODE -ne 0) { throw 'Quick scan failed' }

        $quickLog = Get-Content (Join-Path $work 'AntiVirusLog.txt') -Raw
        if ($quickLog -notmatch 'infected\.bin Infected!') { throw 'Quick scan missed first-region signature' }
        if ($quickLog -notmatch 'late\.bin Infected!') { throw 'Quick scan missed last-region signature' }
        if ($quickLog -notmatch 'empty\.bin clean') { throw 'Quick scan failed on an empty file' }
        if ($quickLog -notmatch 'short\.bin clean') { throw 'Quick scan failed when the signature is larger than the file' }
    }
    finally {
        Pop-Location
    }

    Write-Host 'Antivirus smoke test passed.'
}
finally {
    Remove-Item -Recurse -Force $work -ErrorAction SilentlyContinue
}
