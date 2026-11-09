let () =
  let ns = Consish.create 65535L [ "A"; "B"; "C" ] in
  Array.iter (fun (n, s) -> Printf.printf "%s: %Lu\n" n s) ns.nodes;
  Printf.printf "ns: %Lu\n\n" ns.ns;
  List.iter
    (fun s ->
      let n, sh = Consish.bs ns s in
      Printf.printf "(%S, %Lu)\n" n sh)
    [ "1"; "10"; "100" ]
