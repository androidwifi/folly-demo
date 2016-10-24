namespace apply_tuple {
	struct index_seqeunce {};

	template<std::size_t N, std::size_t... Is>
		struct make_index_sequence : make_index_sequence<N-1, N-1, Is...> {};
	template<std::size_t... Is>
		struct make_index_sequence<0, Is...> : index_seqeunce<Is...> {};

	template <class Tuple>
		using make_index_sequence_from_tuple = 
		make_index_sequence<std::tuple_size<typename std::decay<Tuple>::type>::value>;

	template <class F>
		inline constexpr F&& make_callable(F&& f) { return std::forward<F>(f); }
	template <class M, class C>
		inline constexpr auto make_callable(M(C::*d)) -> decltype(std::mem_fn(d)) {
			return std::mem_fn(d);
		}
}
