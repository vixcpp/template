# Security Policy

## Supported Versions

The following versions of the Vix Template Engine are currently supported with security updates:

| Version | Supported |
| ------- | --------- |
| 0.2.x   | ✅ Yes    |
| 0.1.x   | ❌ No     |

---

## Reporting a Vulnerability

If you discover a security vulnerability, please report it responsibly.

### How to report

- Do **not** open a public issue.
- Send an email to: **security@vixcpp.com** (or project maintainer if email is not available)
- Provide as much detail as possible:
  - description of the vulnerability
  - steps to reproduce
  - potential impact
  - possible fixes (if known)

---

## What to expect

After reporting:

- You will receive an acknowledgment within **48 hours**
- The issue will be investigated and validated
- A fix will be developed and tested
- A patch will be released as soon as possible

---

## Security Principles

The Vix Template Engine is designed with security in mind:

- No dynamic code execution
- Deterministic rendering (no hidden behavior)
- No implicit I/O during rendering
- Strict separation between parsing and execution
- HTML escaping enabled by default

---

## Scope

Security considerations include:

- template injection risks
- unsafe variable rendering
- memory safety issues
- denial of service (DoS) via malformed templates

---

## Disclosure Policy

- Vulnerabilities will be disclosed publicly only after a fix is available
- Credit will be given to the reporter (if desired)

---

## Notes

Security is a shared responsibility.

Always:
- validate user input
- avoid exposing sensitive data in templates
- use safe defaults

---

Thank you for helping make Vix Template Engine safer.

