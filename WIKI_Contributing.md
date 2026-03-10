# Contributing to Whisper LCE

Thank you for your interest in contributing to Whisper LCE! This document provides guidelines for contributing to the project.

## Table of Contents

- [Code of Conduct](#code-of-conduct)
- [How to Contribute](#how-to-contribute)
- [Reporting Bugs](#reporting-bugs)
- [Suggesting Features](#suggesting-features)
- [Pull Request Process](#pull-request-process)
- [Coding Standards](#coding-standards)
- [Development Setup](#development-setup)

---

## Code of Conduct

### Our Standards

- Be respectful and inclusive
- Accept constructive criticism
- Focus on what's best for the community
- Show empathy towards others

### Unacceptable Behavior

- Harassment or discriminatory language
- Trolling or insulting comments
- Publishing others' private information
- Other unprofessional conduct

---

## How to Contribute

### Ways to Contribute

1. **Report Bugs** - Found a bug? Let us know!
2. **Suggest Features** - Have an idea? Share it!
3. **Write Code** - Fix bugs or implement features
4. **Improve Documentation** - Help others understand the project
5. **Test Builds** - Try new features and report issues

---

## Reporting Bugs

### Before Submitting

1. Check if the bug has already been reported
2. Verify you're using the latest version
3. Collect relevant information (logs, screenshots, etc.)

### Bug Report Template

Use the bug report template when creating an issue:

**What happened?**
- Clear description of the bug

**Steps to reproduce**
1. Step one
2. Step two
3. Step three

**Expected behavior**
- What should have happened

**Platform / Build**
- OS: Windows 10/11
- Branch: main
- Commit: (if known)

**Screenshots / Logs**
- Attach relevant files

---

## Suggesting Features

### Before Suggesting

1. Check if the feature has already been requested
2. Review the [Roadmap](WIKI_Roadmap.md) to see if it's planned
3. Consider if it fits the project's goals

### Feature Request Template

Use the feature request template when creating an issue:

**Feature Description**
- Clear description of the feature

**Use Case**
- Why is this feature needed?
- Who would benefit?

**Proposed Implementation**
- How could this be implemented?
- Any technical considerations?

**Alternatives**
- Other ways to achieve the same goal

---

## Pull Request Process

### Before Submitting a PR

1. **Fork the repository**
2. **Create a feature branch** - `git checkout -b feature/your-feature-name`
3. **Make your changes** - Follow coding standards
4. **Test your changes** - Ensure everything works
5. **Commit your changes** - Use clear commit messages
6. **Push to your fork** - `git push origin feature/your-feature-name`
7. **Open a Pull Request** - Use the PR template

### PR Requirements

- [ ] Code compiles without errors
- [ ] Code follows project coding standards
- [ ] Changes are documented (if applicable)
- [ ] No merge conflicts with main branch
- [ ] PR description clearly explains changes

### PR Template

When opening a PR, include:

**Summary**
- Brief description of changes

**Changes Made**
- List of specific changes
- Files modified

**Testing**
- How were changes tested?
- Any known issues?

**Related Issues**
- Fixes #123 (if applicable)

---

## Coding Standards

### General Guidelines

1. **Follow existing code style** - Match the surrounding code
2. **Use meaningful names** - Variables, functions, classes
3. **Comment complex logic** - Help others understand
4. **Keep functions small** - Single responsibility principle
5. **Avoid magic numbers** - Use named constants

### C++ Style Guide

#### Naming Conventions

```cpp
// Classes: PascalCase
class PlayerEntity { };

// Functions: camelCase
void updatePosition() { }

// Variables: camelCase
int playerHealth = 100;

// Constants: UPPER_SNAKE_CASE
#define MAX_PLAYERS 8
const int WORLD_HEIGHT = 256;

// Private members: m_ prefix
class Player {
private:
    int m_health;
    float m_speed;
};
```

#### Code Formatting

```cpp
// Braces on same line for functions
void myFunction() {
    // code here
}

// Braces on new line for classes
class MyClass
{
public:
    void method();
};

// Indentation: 4 spaces (or match existing)
if (condition) {
    doSomething();
}

// Space after keywords
if (x > 0) { }
for (int i = 0; i < 10; i++) { }
while (running) { }
```

#### Comments

```cpp
// Single-line comments for brief explanations
int count = 0; // Initialize counter

/*
 * Multi-line comments for detailed explanations
 * Use for function/class documentation
 */
void complexFunction() {
    // Implementation
}
```

### File Organization

```cpp
// Header file (.h)
#pragma once

// Includes
#include <vector>
#include "OtherClass.h"

// Forward declarations
class ForwardDeclared;

// Class definition
class MyClass {
public:
    MyClass();
    ~MyClass();
    
    void publicMethod();
    
private:
    void privateMethod();
    int m_member;
};
```

```cpp
// Implementation file (.cpp)
#include "MyClass.h"

// Constructor
MyClass::MyClass() : m_member(0) {
}

// Methods
void MyClass::publicMethod() {
    // Implementation
}
```

---

## Development Setup

### Prerequisites

- Visual Studio 2022 (Community or higher)
- Windows 10/11 SDK
- Git

### Setup Steps

1. **Clone the repository**
   ```bash
   git clone https://github.com/Minecraft-Community-Edition/client.git
   cd client
   ```

2. **Install dependencies**
   ```batch
   install_4jlibs.bat
   ```

3. **Open solution**
   - Open `MinecraftConsoles.sln` in Visual Studio

4. **Build**
   - Select Release | x64
   - Build Solution (Ctrl+Shift+B)

5. **Test**
   - Run `Launcher.exe` from output directory

### Development Workflow

1. **Create feature branch**
   ```bash
   git checkout -b feature/my-feature
   ```

2. **Make changes**
   - Edit code
   - Test changes
   - Commit frequently

3. **Commit changes**
   ```bash
   git add .
   git commit -m "Add feature: description"
   ```

4. **Push to fork**
   ```bash
   git push origin feature/my-feature
   ```

5. **Open Pull Request**
   - Go to GitHub
   - Click "New Pull Request"
   - Fill out PR template

---

## Testing Guidelines

### Before Submitting

Test your changes thoroughly:

- [ ] Code compiles without errors
- [ ] Code compiles without warnings (if possible)
- [ ] Game launches successfully
- [ ] Feature works as expected
- [ ] No crashes or freezes
- [ ] No memory leaks (if applicable)

### Testing Checklist

For bug fixes:
- [ ] Bug is reproducible before fix
- [ ] Bug is fixed after changes
- [ ] No new bugs introduced

For new features:
- [ ] Feature works as described
- [ ] Edge cases handled
- [ ] Error handling implemented
- [ ] Performance is acceptable

---

## Documentation

### When to Update Documentation

Update documentation when:
- Adding new features
- Changing existing behavior
- Fixing bugs that affect usage
- Modifying build process

### Documentation Files

- `README.md` - Project overview
- `WIKI_*.md` - Wiki pages
- `*.md` (specific topics) - Detailed guides
- Code comments - Inline documentation

---

## Project Philosophy

### Core Principles

1. **Preserve Legacy Feel** - Keep 1.8 combat, quasi-connectivity, 4J UI
2. **Fix Bugs, Keep Glitches** - Fix crashes, preserve fun exploits
3. **PC-First** - Optimize for PC, not console limitations
4. **Optional Modernization** - New features should be toggleable

### What We Won't Change

- ❌ 1.8 Combat (no attack cooldown)
- ❌ Java Redstone (quasi-connectivity, zero-tick)
- ❌ 4J UI Navigation (looping menus)
- ❌ Fun glitches (overstacked items, block transmutation)

---

## Getting Help

### Resources

- **Wiki**: https://github.com/Minecraft-Community-Edition/client/wiki
- **Issues**: https://github.com/Minecraft-Community-Edition/client/issues
- **Discussions**: https://github.com/Minecraft-Community-Edition/client/discussions

### Questions?

- Open a discussion on GitHub
- Check existing issues and PRs
- Read the documentation

---

## License

By contributing, you agree that your contributions will be subject to the same license as the project (educational/preservation use of leaked source code).

---

## Recognition

Contributors will be recognized in:
- [Credits](WIKI_Credits.md) page
- GitHub contributors list
- Release notes (for significant contributions)

---

**Thank you for contributing to Whisper LCE!**

Your contributions help preserve and improve Minecraft Legacy Console Edition for the community.

---

**Last Updated**: March 10, 2026  
**Maintained By**: M1noa
