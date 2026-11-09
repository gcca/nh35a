let test_hash_s () =
  List.iter
    (fun (s, expected) ->
      let actual = Consish.hash_s s 65535L in
      if actual <> expected then
        failwith
          (Printf.sprintf "hash_s %S = %Lu, want %Lu" s actual expected))
    [
      ("A", 14321L);
      ("B", 8630L);
      ("C", 35588L);
      ("1", 21135L);
      ("10", 41300L);
      ("100", 19522L);
    ]

let test_create () =
  let ns = Consish.create 65535L [ "A"; "B"; "C" ] in
  assert (ns.nodes = [| ("B", 8630L); ("A", 14321L); ("C", 35588L) |])

let test_bs () =
  let ns = Consish.create 65535L [ "A"; "B"; "C" ] in
  List.iter
    (fun (s, n, sh) ->
      let actual = Consish.bs ns s in
      if actual <> (n, sh) then
        failwith (Printf.sprintf "unexpected lookup for %S" s))
    [
      ("1", "C", 21135L);
      ("10", "B", 41300L);
      ("100", "C", 19522L);
      ("", "B", 5970L);
      ("A", "A", 14321L);
      ("B", "B", 8630L);
      ("C", "C", 35588L);
    ]

let test_unsigned_ring () =
  let ns = Consish.create Int64.minus_one [ "A"; "B"; "C" ] in
  assert (Array.map fst ns.nodes = [| "C"; "A"; "B" |]);
  assert
    (Consish.hash_s "B" ns.ns = Int64.of_string "0u18185419767222700475");
  List.iter
    (fun (s, n) -> assert (fst (Consish.bs ns s) = n))
    [ ("1", "B"); ("10", "B"); ("100", "B"); ("A", "A"); ("B", "B");
      ("C", "C") ]

let test_single_node_and_collisions () =
  let single = Consish.create 65535L [ "A" ] in
  List.iter
    (fun s -> assert (fst (Consish.bs single s) = "A"))
    [ ""; "A"; "1"; "10"; "100" ];
  let collisions = Consish.create 1L [ "A"; "B"; "C" ] in
  assert (Consish.bs collisions "10" = ("A", 0L))

let expect_invalid_arg message f =
  match f () with
  | _ -> failwith ("expected Invalid_argument: " ^ message)
  | exception Invalid_argument actual -> assert (actual = message)

let test_invalid_inputs () =
  expect_invalid_arg "no elements" (fun () ->
      Consish.bs (Consish.create 65535L []) "1");
  expect_invalid_arg "zero ring size" (fun () -> Consish.create 0L []);
  expect_invalid_arg "zero ring size" (fun () -> Consish.hash_s "A" 0L)

let () =
  test_hash_s ();
  test_create ();
  test_bs ();
  test_unsigned_ring ();
  test_single_node_and_collisions ();
  test_invalid_inputs ()
