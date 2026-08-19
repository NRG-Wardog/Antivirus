# AntiVirus

[![CI](https://github.com/NRG-Wardog/Antivirus/actions/workflows/ci.yml/badge.svg)](https://github.com/NRG-Wardog/Antivirus/actions/workflows/ci.yml)

**Early C signature-based antivirus scanner, hardened with recursive traversal, safer file handling, and deterministic Windows CI.**

This repository started as an educational C implementation of a classic antivirus primitive: recursively walking a directory, reading files as binary data, searching for a known byte signature, supporting normal/quick scan modes, and logging per-file results.

It remains a historical systems/security project, but the implementation is now maintained well enough to serve as credible evidence of low-level C, filesystem, binary-scanning, and correctness work. The later evolution of the security work is [AYDO](https://github.com/NRG-Wardog/Aydo).

---

## What It Implements

- Recursive directory traversal, including nested folders
- Binary signature matching with explicit bounds handling
- Full scan mode
- Quick scan mode with first/middle/last region coverage
- Empty-file and oversized-signature handling
- Per-file clean/infected reporting
- Deterministic scan-log generation
- Windows directory compatibility through the bundled `dirent.h`
- Automated MSVC build + integration smoke test in GitHub Actions

This is a learning implementation of a signature scanner, not a modern production antivirus engine.

---

## Scanner Flow

```text
Directory
   |
   +-- recurse into subdirectories
   |
   +-- file -> read binary bytes
                 |
                 +-- Normal: scan full buffer
                 |
                 +-- Quick: inspect bounded regions
                              |
                              +-- signature match -> Infected
                              +-- no match        -> Clean

Results -> console + AntiVirusLog.txt
```

---

## Repository Layout

```text
Antivirus/
├── antivirus_files/
│   ├── prog.c              # scanner implementation
│   ├── dirent.h            # Windows-compatible directory traversal support
│   ├── KittenVirusSign     # example signature fixture
│   ├── AntiVirusLog.txt    # historical example output
│   └── files/              # sample scan fixtures
├── tests/
│   └── smoke_test.ps1      # deterministic recursive-scan integration test
├── .github/workflows/
│   └── ci.yml              # Windows/MSVC CI
├── LICENSE
└── README.md
```

---

## Build

The project is Windows-oriented because it carries a Visual-Studio-compatible `dirent.h` implementation.

From an MSVC developer shell:

```powershell
cl /nologo /W4 /TC antivirus_files\prog.c /Fe:antivirus.exe
```

A GCC/MinGW build is also possible with an appropriate Windows toolchain.

---

## Run

```powershell
.\antivirus.exe <directory_path> <signature_file_path>
```

The program asks for the scan mode:

- `0` — normal/full scan
- any other integer — quick scan

Results are printed and written to `AntiVirusLog.txt`.

---

## Validation

The CI smoke test builds the scanner with MSVC, creates temporary clean/infected fixtures including a **nested infected file**, runs both normal and quick scan modes, and verifies the generated log contains the expected detections.

Run locally from an MSVC-enabled PowerShell session:

```powershell
.\tests\smoke_test.ps1
```

The test specifically guards behavior that is easy to regress in a file scanner: recursive traversal, clean/infected classification, and signature placement near different regions of a file.

---

## Engineering Concepts Demonstrated

- C file I/O and explicit resource management
- Binary byte-pattern search
- Recursive filesystem traversal
- Defensive bounds checking
- Scan-mode tradeoffs
- Deterministic logging/reporting
- Integration testing of native Windows code
- Early endpoint-security reasoning

---

## Limitations

- Signature matching only; no behavioral, heuristic, reputation, or sandbox analysis
- No real-time file monitoring
- No quarantine or remediation pipeline
- No signed-update mechanism
- No parallel scanning or production-scale performance engineering
- Quick scan is an educational optimization, not an efficacy guarantee
- Intended as an educational scanner, not malware protection for real endpoints

For the later evolution of this work, see **[AYDO](https://github.com/NRG-Wardog/Aydo)**, which expands into endpoint services, telemetry, detection pipelines, backend infrastructure, and isolated dynamic analysis.

---

## License

See [LICENSE](LICENSE).
