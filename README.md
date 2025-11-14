# Bcryptenum

This is a small C program that enumerates all [`BCrypt`](https://learn.microsoft.com/en-us/windows/win32/seccng/cng-portal) (aka Cryptography API: Next Generation (`CNG`)) algorithms on Windows and prints them on the console.

It is useful when one wants to use a `BCrypt` algorithm and needs to know which algorithms are supported on a machine, as this depends on the Windows version.
E.g. the [SHA-3](https://en.wikipedia.org/wiki/SHA-3) hash functions are supported since Windows 11 24H2 (Version 10.0.26100.2161), 9 years after the NIST published the [SHA-3 standard](https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.202.pdf)!

Since [.NET](https://dotnet.microsoft.com/en-us/) – for some reason that completely eludes me – uses the cryptographic functions of the platform OS the list of supported algorithms is relevant for .NET, as well.

## Contributing
Feel free to submit a pull request with new features, improvements on tests or documentation and bug fixes.

## Contact
Frank Schwab ([Mail](mailto:xformer.github@proton.me "Mail"))

## License
Algoenum is released under the Apache V2 license. See "LICENSE" for details.
