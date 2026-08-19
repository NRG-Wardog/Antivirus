# AntiVirus

**Early C signature-based antivirus scanner — historical systems/security project.**

This repository contains an earlier C project that explores the basic mechanics behind signature-based malware scanning: recursively walking a directory, reading file contents, searching for a known byte signature, supporting a reduced quick-scan mode, and writing scan results to a log.

It is intentionally kept as a historical project. The security work that followed this project later expanded into the much broader Windows endpoint-protection architecture in [AYDO](https://github.com/NRG-Wardog/Aydo).

---

## What It Implements

- Recursive directory traversal
- Signature-based file scanning
- Full scan mode
- Quick scan mode
- Per-file clean/infected result reporting
- Scan log generation
- Windows-oriented directory compatibility through the bundled `dirent.h`

This is a learning implementation of a classic antivirus primitive, not a modern production antivirus engine.

---

## Repository Layout

```text
Antivirus/
├── antivirus_files/
│   ├── prog.c              # scanner implementation
│   ├── dirent.h            # Windows-compatible directory traversal support
│   ├── KittenVirusSign     # example signature fixture
│   ├── AntiVirusLog.txt    # example scan output
│   └── files/              # sample scan fixtures
├── LICENSE
└── README.md
```

---

## Build

From the project directory containing `prog.c`:

```bash
cd antivirus_files
gcc prog.c -o antivirus
```

The source was written for a Windows-oriented development environment. Compiler and path adjustments may be required on other platforms.

---

## Run

The program prompts for the directory to scan, the signature file, and the scan mode.

Example project fixtures are available under `antivirus_files/files/` with the example signature at `antivirus_files/KittenVirusSign`.

A scan produces console output and writes an `AntiVirusLog.txt` report.

---

## Engineering Concepts Demonstrated

- C file I/O and memory handling
- Byte-pattern search
- Recursive filesystem traversal
- Basic scan-mode optimization
- Deterministic logging/reporting
- Early endpoint-security reasoning

---

## Limitations

- Signature matching only; no behavioral, heuristic, reputation, or sandbox analysis
- No real-time file monitoring
- No quarantine or remediation pipeline
- No signed-update mechanism
- No parallel scanning or production-scale performance engineering
- Intended as an educational project, not malware protection for real endpoints

For the later evolution of this work, see **[AYDO](https://github.com/NRG-Wardog/Aydo)**, which expands into endpoint services, telemetry, detection pipelines, backend infrastructure, and isolated dynamic analysis.

---

## License

See [LICENSE](LICENSE).
