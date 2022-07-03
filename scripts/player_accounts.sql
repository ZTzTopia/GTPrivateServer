create table player.accounts
(
    user_id     integer default nextval('player.accounts_id_seq'::regclass) not null,
    id_         uuid    default gen_random_uuid()                           not null,
    name        varchar,
    guest       boolean,
    register_ip inet,
    login_ip    inet,
    mac         macaddr,
    password    varchar,
    rid         bytea,
    wk          bytea
)
    using ???;

alter table player.accounts
    owner to zenta;

