# Contributing to Molecular Dynamics

___

## Git workflow

1. Make a new branch from master branch (See branch types style guide)
    ``` 
    $ git checkout master
    $ git pull
    $ git checkout -b <branchtype>/<word describing your branch>
   
    # for example
      $ git checkout -b feat/commandline-interface
    ```
2. Make your changes
3. Format code
4. Commit your stuff (See commit message style guide)
5. Push it to remote
    ``` 
    $ git push origin <branchname>
  
    # for example
      $ git push origin feat/commandline-interface
    ```
6. Loop back to step 2 if necassary
7. Click on the link in the command output
8. Add us as reviewers and you as assignee and open pull request

## Styleguide

### Commit and branch types:

- **feat**: The new feature you're adding to a particular application
- **fix**: A bug fix
- **style**: Feature and updates related to styling (Code format)
- **refactor**: Refactoring a specific section of the codebase
- **test**: Everything related to testing
- **docs**: Everything related to documentation
- **chore**: Regular code maintenance(CI, etc.)

### Commit Messages

```
<type>: <subject>
<blank line(only needed if there is a description)>
<description(OPTIONAL)>
```

- `<type>`: One of the specified types in commit and branch types
- `<subject>`: Short description what you did
    - Start with a small character
    - Use the imperative mood
    - Don't end with a `.`
    - Keep it short not more than 60 characters
- `<description>`: OPTIONAL if there is more to say
    - Use the imperative mood
    - Don't end with a `.`
    - Add a linebreak after max. 70 characters

##### Examples

- fix: set exclude doc_doxygen from all
- refactor: change the commandline parsing to used std function
- fix: change int to size_t
- feat: add constructor to ParticleContainer
