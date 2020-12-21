# Anopr

Anopr is a set of modules that work together to authenticate IRC chatters against a [Composr CMS](https://compo.sr/) database. It consists of two main pieces:

- A C++ [Anope](https://www.anope.org/) module
- A set of PHP scripts

This system is necessary because Composr uses PHP's `password_hash` and `password_verify` functions, which Anope's SQL authentication module cannot handle.

## Setup & Configuration

To use Anopr, you must have a Composr installation with user accounts set up, and an IRC server that you plan to use Anope services with.

1. Download the [source code for Anope](https://github.com/anope/anope/releases/).
2. Place `m_composr_authentication.cpp` in the `modules/third` directory of Anope.
3. Run Configure, build, and install Anope.
    - `./Configure`
    - `cd build`
    - `make`
    - `make install`
4. Place a module configuration block at the end of Anope's `modules.conf` configuration file.
    - See `example-anope-config.conf` for a skeleton configuration.
5. Adjust the PHP scripts to point to the appropriate SQL database (including the table, if it's been customized from Composr's default), 
   and SSL/TLS certificates if you are connecting to Composr's SQL database over SSL/TLS.

## Usage

Once set up using the steps above, users who have registered an account on your Composr CMS installation can 
join your IRC server and authenticate with the following command (assuming default NickServ configuration in Anope):

```
/msg NickServ IDENTIFY (password)
```

...or, if connected with a different nick than the Composr username:

```
/msg NickServ IDENTIFY (username) (password)
```

## Supporting development

Anopr was developed by Jacob Kauffmann at Nerd on the Street. 
If you find this module useful, you can support development by joining the Nerd Club at https://nerdclub.nots.co.

## License

Anopr is licensed under the terms of the GNU GPLv3. The full text of this license is included in the LICENSE file.

Anopr includes code from Anope (`m_composr_authentication.cpp` is based on `m_sql_authentication.cpp` from Anope.) Anope is licensed under the GNU GPLv2 or later.

Anopr does not contain any code from Composr (it only accesses the database as an SQL client.)
