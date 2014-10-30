Post-install
-----------------------

1. See [example.bashrc](examples/example.bashrc) for instructions on how to add some
   useful shell aliases that provides even tighter integration between
   drush and bash. You may source this file directly into your shell by adding to
   your .bashrc (or equivalent): source /path/to/drush/examples/example.bashrc

1. If you didn't source it the step above, see top of
   [drush.complete.sh](drush.complete.sh) file for instructions adding bash completion for drush
   command to your shell.  Once configured, completion works for site aliases,
   command names, shell aliases, global options, and command-specific options.

1. Optional. If [drush.complete.sh](drush.complete.sh) is being sourced (ideally in
   bash_completion.d), you can use the supplied __drush_ps1() sh function to
   add your current drush site (set with `drush use @sitename`) to your PS1
   prompt like so:

      ``` bash
      if [ "\$(type -t __git_ps1)" ] && [ "\$(type -t __drush_ps1)" ]; then
        PS1='\u@\h \w$(__git_ps1 " (%s)")$(__drush_ps1 "[%s]")\$ '
      fi
      ```

   Putting this in a .bashrc/.bash_profile/.profile would produce this prompt:

     `msonnabaum@hostname ~/repos/drush (master)[@sitename]$`

1. Help the Drush development team by sending anonymized usage statistics.  To automatically send usage data, please add the following to a .drushrc.php file:

       ```php
       $options['drush_usage_log'] = TRUE;
       $options['drush_usage_send'] = TRUE;
       ```

     Stats are usually logged locally and sent whenever log file exceeds 50Kb.
     Alternatively, one may disable automatic sending and instead use
     `usage-show` and `usage-send` commands to more carefully send data.


MAMP configuration
------------------

Users of MAMP will need to manually specify in their PATH which version of php
and MySQL to use in the command line interface. This is independent of the php
version selected in the MAMP application settings.  Under OS X, edit (or create
if it does not already exist) a file called .bash_profile in your home folder.

To use php 5.3.x, add this line to .bash_profile:

    export PATH="/Applications/MAMP/Library/bin:/Applications/MAMP/bin/php5.3/bin:$PATH"

If you want to use php 5.4.x, add this line instead:

    export PATH="/Applications/MAMP/Library/bin:/Applications/MAMP/bin/php5.4/bin:$PATH"
    
If you use MAMP 3 (php 5.5.14 by default) and want to use php 5.5.x , add this line instead:

    export PATH="/Applications/MAMP/Library/bin:/Applications/MAMP/bin/php/php5.5.14/bin:$PATH"

If you have MAMP v.1.84 or lower, this configuration will work for both versions
of PHP:

    export PATH="/Applications/MAMP/Library/bin:/Applications/MAMP/bin/php5/bin:$PATH"

If you have done this and are still getting a "no such file or directory" error
from PDO::__construct, try this:

```bash
  sudo mkdir /var/mysql
  sudo ln -s /Applications/MAMP/tmp/mysql/mysql.sock /var/mysql/mysql.sock
```

Additionally, you may need to adjust your php.ini settings before you can use
drush successfully. See CONFIGURING PHP.INI below for more details on how to
proceed.

Configuration for other AMP stacks
-----------------------------------------------

Users of other Apache distributions such as XAMPP, or Acquia's Dev Desktop will
want to ensure that its php can be found by the command line by adding it to
the PATH variable, using the method in 3.b above. Depending on the version and
distribution of your AMP stack, PHP might reside at:

Path                                       | Application
-----                                      | ----
/Applications/acquia-drupal/php/bin        | Acquia Dev Desktop (Mac)
/Applications/xampp/xamppfiles/bin         | XAMP (Mac)
/opt/lampp/bin                             | XAMPP (Windows)

Additionally, you may need to adjust your php.ini settings before you can use
drush successfully. See CONFIGURING PHP.INI below for more details on how to
proceed.

Specify PHP version for Drush
--------------------------

  If you want to run Drush with a specific version of php, rather than the
  php defined by your shell, you can add an environment variable to your
  the shell configuration file called .profile, .bash_profile, .bash_aliases,
  or .bashrc that is located in your home folder:

    export DRUSH_PHP='/path/to/php'

php.ini configuration
-------------------

Usually, php is configured to use separate php.ini files for the web server and
the command line. Make sure that Drush's php.ini is given as much memory to
work with as the web server is; otherwise, Drupal might run out of memory when
Drush bootstraps it.

To see which php.ini file Drush is using, run:

    $ drush status

To see which php.ini file the webserver is using, use the phpinfo() function in
a .php web page.  See http://drupal.org/node/207036.

If Drush is using the same php.ini file as the web server, you can create a
php.ini file exclusively for Drush by copying your web server's php.ini file to
the folder $HOME/.drush or the folder /etc/drush.  Then you may edit this file
and change the settings described above without affecting the php enviornment
of your web server.

Alternately, if you only want to override a few values, copy [example.drush.ini](examples/example.drush.ini)
from the /examples folder into $HOME/.drush or the folder /etc/drush and edit
to suit.  See comments in example.drush.ini for more details.

You may also use environment variables to control the php settings that Drush
will use.  There are three options:

```bash
export PHP_INI='/path/to/php.ini'
export DRUSH_INI='/path/to/drush.ini'
export PHP_OPTIONS='-d memory_limit="128M"'
```

In the case of PHP_INI and DRUSH_INI, these environment variables specify the
full path to a php.ini or drush.ini file, should you wish to use one that is
not in one of the standard locations described above.  The PHP_OPTIONS
environment variable can be used to specify individual options that should
be passed to php on the command line when Drush is executed.

Drush requires a fairly unrestricted php environment to run in.  In particular,
you should insure that safe_mode, open_basedir, disable_functions and
disable_classes are empty.  If you are using php 5.3.x, you may also need to
add the following definitions to your php.ini file:

```ini
magic_quotes_gpc = Off
magic_quotes_runtime = Off
magic_quotes_sybase = Off
```

PHP 5.5 configuration
-----------------------------

If you are running on Linux, you may find that you need
the php5-json package.  On Ubuntu, you can install it via:

`apt-get install php5-json`

Setting default options
-----------

For multisite installations, use the -l option to target a particular site.  If
you are outside the Drupal web root, you might need to use the -r, -l or other
command line options just for Drush to work. If you do not specify a URI with
-l and Drush falls back to the default site configuration, Drupal's
$GLOBALS['base_url'] will be set to http://default.  This may cause some
functionality to not work as expected.

    $ drush -l http://example.com pm-update

If you wish to be able to select your Drupal site implicitly from the
current working directory without using the -l option, but you need your
base_url to be set correctly, you may force it by setting the uri in
a drushrc.php file located in the same directory as your settings.php file.

**sites/default/drushrc.php:**
```
$options['uri'] = "http://example.com";
```

Related Options:
  ```
  -r <path>, --root=<path>      Drupal root directory to use
                                (defaults to current directory or anywhere in a
                                Drupal directory tree)
  -l <uri> , --uri=<uri>        URI of the Drupal site to use
  -v, --verbose                 Display verbose output.
  ```

Run-time configuration
-----------

Inside the [examples](examples) directory you will find some example files to help you get
started with your Drush configuration file (example.drushrc.php), site alias
definitions (example.aliases.drushrc.php) and Drush commands
(sandwich.drush.inc). You will also see an example 'policy' file which can be
customized to block certain commands or arguments as required by your
organization's needs.

drushrc.php
-----------

If you get tired of typing options all the time you can contain them in a
drushrc.php file. Multiple Drush configuration files can provide the
flexibility of providing specific options in different site directories of a
multi-site installation. See [example.drushrc.php](examples/example.drushrc.php) for examples and installation
details.

Site aliases
------------

Drush lets you run commands on a remote server, or even on a set of remote
servers.  Once defined, aliases can be references with the @ nomenclature, i.e.

```bash
# Synchronize staging files to production
$ drush rsync @staging:%files/ @live:%files
# Syncronize database from production to dev, excluding the cache table
$ drush sql-sync --structure-tables-key=custom @live @dev
```

See http://drupal.org/node/670460 and [example.aliases.drushrc.php](examples/example.aliases.drushrc.php) for more
information.

Commands
--------

Drush can be extended to run your own commands. Writing a Drush command is no harder than writing simple Drupal modules, since they both follow the same structure.

See [sandwich.drush.inc](examples/sandwich.drush.inc) for a quick tutorial on Drush command files.  Otherwise, the core commands in Drush are good models for your own commands.

You can put your Drush command file in a number of places:

  1. In a folder specified with the --include option (see `drush topic
     docs-configuration`).
  1. Along with one of your enabled modules. If your command is related to an
     existing module, this is the preferred approach.
  1. In a .drush folder in your HOME folder. Note, that you have to create the
     .drush folder yourself.
  1. In the system-wide Drush commands folder, e.g. /usr/share/drush/commands.
  1. In Drupal's /drush or sites/all/drush folders. Note, that you have to create the
     drush folder yourself.

In any case, it is important that you end the filename with ".drush.inc", so that Drush can find it.
