# TFTP

## Procedimiento de transmisión

1. Solicitud de escritura/lectura, también es solicitud de conexión
2. El archivo es transmitido en bloques de 512 bytes
3. Cada bloque se responde con un ACK
4. Un bloque menor a los 512 bytes indica fin de transmisión
5. Timeout -> retransmisión (de data o de ACK)

## Tipos de paquete

| opcode | operación | descripcion |
| --- | --- | --- |
| 1 | RRQ | Read request |
| 2 | WRQ | Write request |
| 3 | DATA | Data |
| 4 | ACK | Acknowledgement |
| 5 | ERROR | Error |

### Paquete RRQ/WRQ

| 2 bytes | string | 1 byte | string | 1 byte |
| --- | --- | --- | --- | --- | 
| opcode | Filename | 0 | Mode | 0 |

### Paquete DATA

| 2 bytes | 2 bytes | n bytes |
| --- | --- | --- |
| opcode | Block # | Data |

### Paquete ACK

| 2 bytes | 2 bytes |
| --- | --- | 
| opcode | Block # | 

### Paquete ERROR

| 2 bytes | 2 bytes | string | 1 byte |
| --- | --- | --- | --- |
| opcode | ErrorCode | ErrMsg | 0 |

## Modos de transmisión

- netascii
- octet
- mail

## Error Codes

0. Not defined
1. File not found
2. Disk full
3. Access violation
4. Illegal TFTP operation
5. Unknown transfer ID
6. File already exists
7. No such user


## Funciones necesitadas server side


## Multiples clientes

- Identificar receive from con el puerto/ip de origen para determinar el cliente
- NO se usa accept, solo un socket que atiende a muchos clientes
- lista de clientes
- NO usar fork xke los hijos compartirian el unico socket y sería caos total
- no usar loop porque es bloqueante, tiene que ser máquina de estados
- maquina de estados tendría
  - estado cliente: ip cliente, puerto cliente, file_fd, offset(block num)
  - estructura hash (unordered map) hasheado con clave ip puerto (uint32, uint16) se puede mantener en network order (no nstoh)