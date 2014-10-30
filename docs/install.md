# Installation

This section provides detailed information on how to correctly install Drush on your system.

## Choosing the right version

Each version of Drush supports multiple Drupal versions:

- Drush 6 is the recommended version
- Drupal 8 requires Drush 7

| Drush Version | Branch                                           | PHP         | Compatible Drupal versions | Code Status                                                         |
| ------------- | ------------------------------------------------ | ----------- | -------------------------- | ------------------------------------------------------------------- |
| Drush 7       | [master](https://travis-ci.org/drush-ops/drush)  | 5.3.0+      | D6, D7, D8                 | <img src="https://travis-ci.org/drush-ops/drush.svg?branch=master"> |
| Drush 6       | [6.x](https://travis-ci.org/drush-ops/drush)     | 5.3.0+      | D6, D7                     | <img src="https://travis-ci.org/drush-ops/drush.svg?branch=6.x">    |
| Drush 5       | [5.x](https://travis-ci.org/drush-ops/drush)     | 5.2.0+      | D6, D7                     | Unsupported                                                         |
| Drush 4       | 4.x                                              | 5.2.0+      | D5, D6, D7                 | Unsupported                                                         | 
| Drush 3       | 3.x                                              | 5.2.0+      | D5, D6                     | Unsupported                                                         |

Drush comes with a full test suite powered by [PHPUnit](https://github.com/sebastianbergmann/phpunit). Each commit gets tested by the awesome [Travis.ci continuous integration service](https://travis-ci.org/drush-ops/drush).

Requirements
-----------

* Drush commands that work with git require git 1.7 or greater.
* Drush works best on a Unix-like OS (Linux, OS X)
* Most Drush commands run on Windows.  See INSTALLING DRUSH ON WINDOWS, below.

Install / Update
------------------

### Install via Composer (recommended)

* [Install Composer globally](http://getcomposer.org/doc/00-intro.md#system-requirements) (if needed).
* Make sure Composer's global bin directory is on the system PATH (recommended):

        sed -i '1i export PATH="$HOME/.composer/vendor/bin:$PATH"' $HOME/.bashrc
        source $HOME/.bashrc

* To install Drush 6.x (stable):

        composer global require drush/drush:6.*

* To install Drush 7.x (dev) which is required for Drupal 8:

        composer global require drush/drush:dev-master

* To update to a newer version (what you get depends on your specification in ~/.composer/composer.json):

        composer global update
        
* To install for all users on the server:

        curl -sS https://getcomposer.org/installer | php
        mv composer.phar /usr/local/bin/composer
        ln -s /usr/local/bin/composer /usr/bin/composer

        git clone https://github.com/drush-ops/drush.git /usr/local/src/drush
        cd /usr/local/src/drush
        git checkout 7.0.0-alpha5  #or whatever version you want.
        ln -s /usr/local/src/drush/drush /usr/bin/drush
        composer install
        drush --version

* Alternate commands to install some other variant of Drush:

        # Install a specific version of Drush, e.g. Drush 6.1.0
        composer global require drush/drush:6.1.0
        # Master branch as a git clone. Great for contributing back to Drush project.
        composer global require drush/drush:dev-master --prefer-source

[Fuller explanation of the require command.](http://getcomposer.org/doc/03-cli.md#require)

**Tips:**

If Drush cannot find an autoloaded class, run `composer self-update`. Drush often
tracks composer changes closely, so you may have some problems if you are not
running a recent version.

If composer cannot find a requirement, and suggests that *The package is not available in a stable-enough version according to your minimum-stability setting*, then place the following
in `$HOME/.composer/composer.json`:
```
{
  "minimum-stability": "dev"
}
```
Merge this in with any other content that may already exist in this file.

See the POST-INSTALL section for configuration tips.

### Manual install

1. Place the uncompressed drush.tar.gz, drush.zip, or cloned git repository in a directory that is outside of your web root.
1. Make the 'drush' command executable:

    `$ chmod u+x /path/to/drush/drush`

1. Configure your system to recognize where Drush resides. There are 3 options:
    1. Create a symbolic link to the Drush executable in a directory that is already in your PATH, e.g.:

         `$ ln -s /path/to/drush/drush /usr/bin/drush`

    1. Explicitly add the Drush executable to the PATH variable which is defined in the the shell configuration file called .profile, .bash_profile, .bash_aliases, or .bashrc that is located in your home folder, i.e.:

           `export PATH="$PATH:/path/to/drush:/usr/local/bin"`

     Your system will search path options from left to right until it finds a result.

    1. Add an alias for drush (this method can also be handy if you want to use 2 versions of Drush, for example Drush 5 or 6 (stable) for Drupal 7 development, and Drush 7 (master) for Drupal 8 development).
     To add an alias to your Drush 7 executable, add this to you shell configuration file (see list in previous option):
         `$ alias drush-master=/path/to/drush/drush`

    For options 2 and 3 above, in order to apply your changes to your current session, either log out and then log back in again, or re-load your bash configuration file, i.e.:

      `$ source .bashrc`

    NOTE: If you do not follow this step, you will need to inconveniently run Drush commands using the full path to the executable "/path/to/drush/drush" or by navigating to /path/to/drush and running "./drush". The -r or -l options will be required (see USAGE, below).

1. Test that Drush is found by your system:

     `$ which drush`

1. From Drush root, run Composer to fetch dependencies.

     `$ composer install`

See the POST-INSTALL section for configuration tips.

Installing on Windows
---------------------

Windows support has improved, but is still lagging. For full functionality,
consider using on Linux/Unix/OSX using Virtualbox or other virtual machine.

There is a [Windows msi installer](https://github.com/drush-ops/drush/releases/download/6.0.0/Drush-6.0-2013-08-28-Installer-v1.0.21.msi).

Whenever the documentation or the help text refers to 'drush [option]
<command>' or something similar, 'drush' may need to be replaced by
'drush.bat'.

Additional Drush Windows installation documentation can be found at
http://drupal.org/node/594744.

Most Drush commands will run in a Windows CMD shell or PowerShell, but the
Git Bash shell provided by the 'Git for Windows' installation is the preferred
shell in which to run Drush commands. For more information on "Git for Windows'
visit http://msysgit.github.com/.

When creating aliases for Windows remote machines, pay particular attention to
information presented in the example.aliases.drushrc.php file, especially when
setting values for 'remote-host' and 'os', as these are very important when
running Drush rsync and Drush sql-sync commands.
