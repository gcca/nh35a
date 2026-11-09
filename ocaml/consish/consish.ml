let hash_s s ns =
  if ns = 0L then invalid_arg "zero ring size";
  let d = Digest.string s in
  let dl = String.get_int64_be d 0 in
  let dh = String.get_int64_be d 8 in
  Int64.unsigned_rem (Int64.logxor dh dl) ns

type t = {
  ns : int64;
  nodes : (string * int64) array;
}

let create ns names =
  if ns = 0L then invalid_arg "zero ring size";
  let nodes = Array.of_list (List.map (fun n -> (n, hash_s n ns)) names) in
  Array.stable_sort
    (fun (_, a) (_, b) -> Int64.unsigned_compare a b)
    nodes;
  { ns; nodes }

let bs t s =
  let len = Array.length t.nodes in
  if len = 0 then invalid_arg "no elements";
  let sh = hash_s s t.ns in
  let rec lower_bound lo hi =
    if lo = hi then lo
    else
      let mid = lo + ((hi - lo) / 2) in
      let _, h = t.nodes.(mid) in
      if Int64.unsigned_compare h sh < 0 then lower_bound (mid + 1) hi
      else lower_bound lo mid
  in
  let i = lower_bound 0 len in
  let n, _ = t.nodes.(if i = len then 0 else i) in
  (n, sh)
