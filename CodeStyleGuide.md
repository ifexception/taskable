# Taskable Code Style Guide

## Introduction

This document serves as a guide for code style to create a uniform coding habit to ease readability and maintenance.

## Files

File names are always lowercase. Header files end with `.h` and source files end with `.cpp`.

All files must contain the shortened license snippet as the first entry of _each file_.

Dialog files end with the suffix `dlg`. Model files end with the suffix `model`. Data files end with the suffix `data`. Wizard files end the suffix `wizard`.

## Comments

Block comment style is preferred. Including for single line comments

## Declarations

### Class

PascalCase naming. No word shortening, use full words

### Variables

Class variable names:

- mVariableName: Ordinary class variables
- pVariableName: Pointer class variables (includes smart pointers)
- bVariableName: Boolean class variables
- VariableName: Static class variables

### Functions

PascalCase
Clear on intent
Break up functionality into segments for readability

## Whitespace

Should be handled by the `.clang-format` file.

## clang-format

Formatting by clang-format take precedence. There are exceptions for disabling the clang-format file. See source files for the exception cases

## Spaces

Spaces are used throughout with indentation of 4
