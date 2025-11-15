  # Project Instructions for Claude Code

  ## Git Commit Messages
  - DO NOT add "🤖 Generated with [Claude Code]" footer
  - DO NOT add "Co-Authored-By: Claude"
  - Keep commit messages clean and standard
  - Use simple language for commit messages, no fancy words

  ## Testing Workflow
  - DO NOT run builds - just write tests and push
  - CI will catch build errors
  - Use 2025 copyright for new files

  ## Code Coverage
  - Exclude 3rd party code (see .codecov.yml)
  - Tests go in separate .test.cpp files
  - Include test files in unity builds